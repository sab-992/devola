#include <core/http/server.hpp>


http_n::server_n::Basic::Basic(const std::string& name, uint16_t port)
: m_ioContext(), m_workGuard(asio::make_work_guard(m_ioContext)), m_sslContext(asio::ssl::context::tls_server), m_port(port), m_signals(m_ioContext, SIGINT, SIGTERM), m_threadPoolSize(0) {
    m_light = log_n::Light::instance();
    m_threadRegistry = thread_n::Registry::instance();

    addExtraLogInformation(name);
    m_light->log(log_n::Level_en::SPECIAL, m_extraLogInformation, "Starting", "...");

    m_http = std::make_unique<Http>(&m_ioContext);
    m_sslContext.set_options(asio::ssl::context::no_sslv2 |
                             asio::ssl::context::no_sslv3);
    SSL_CTX_set_alpn_select_cb(m_sslContext.native_handle(), alpnSelectCallback, nullptr);
    const std::string& secretsDirPath = std::format("{}/server/settings/secrets", ROOT_DIRECTORY);
    m_sslContext.use_certificate_chain_file(std::format("{}/cert.pem", secretsDirPath));
    m_sslContext.use_private_key_file(std::format("{}/key.pem", secretsDirPath), asio::ssl::context::pem);
    m_signals.async_wait([&](const auto&, const auto&){ stop(); });
}

http_n::server_n::Basic::~Basic() {
    stop();
    clean();
}

void http_n::server_n::Basic::addExtraLogInformation(const std::string& element) {
    m_extraLogInformation.emplace_back(element);
}

int http_n::server_n::Basic::alpnSelectCallback(SSL*, const unsigned char** out, unsigned char* outlen,
                                                      const unsigned char* in, unsigned int inlen, void* /*arg*/) {
    static const unsigned char supported[] = { 8, 'h','t','t','p','/','1','.','1' };
    int status = SSL_select_next_proto(const_cast<unsigned char**>(out), outlen, supported, sizeof(supported), in, inlen);

    if (status != OPENSSL_NPN_NEGOTIATED)
        return SSL_TLSEXT_ERR_ALERT_FATAL;

    return SSL_TLSEXT_ERR_OK;
}

void http_n::server_n::Basic::clean() {
    for (const auto& id : m_threadIds)
        m_threadRegistry->join(id);

    m_threadIds.clear();
}

asio::awaitable<void> http_n::server_n::Basic::handleClient(asio::ip::tcp::socket&& socket) {
    using namespace http_n;
    try {
        auto session = Session::create(asio::ssl::stream<asio::ip::tcp::socket>(std::move(socket), m_sslContext));

        #ifdef DEBUG_MODE_ENABLED
            session->setExtraLogInformation(m_extraLogInformation);
        #endif

        co_await session->handshake();
        Request request = co_await session->read();
        const std::string& method = request.method();
        const std::string& endpoint = request.APIEndpoint();

        if (not m_endpoints.contains(method))
            session->error(network_n::Code::NOT_FOUND);
        else {
            const boundHandler_t& handler = m_endpoints[method].route(endpoint);
            if (not handler)
                session->error(network_n::Code::NOT_ALLOWED);
            else
                session->write(co_await handler(*session, request));
        }

    } catch (std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, m_extraLogInformation, "Exception in worker thread. Function:", std::format("[{}]", FUNCTION_SIGNATURE), "Reason: ", e.what());
    }
}

asio::awaitable<void> http_n::server_n::Basic::listen() {
    using tcp = asio::ip::tcp;

    auto executor = co_await asio::this_coro::executor;
    tcp::acceptor acceptor(executor, { tcp::v4(), m_port });
    while(m_isRunning)
        m_http->spawn(handleClient(co_await acceptor.async_accept(asio::use_awaitable)), asio::detached);
}

void http_n::server_n::Basic::run() {
    if (m_endpoints.empty())
        throw Exception("No endpoints specified");

    m_isRunning = true;
    startSequence();

    startThreadPool();

    m_light->log(log_n::Level_en::INFO, m_extraLogInformation, "Listening at port:", m_port);
    m_http->spawn(listen(), asio::detached);

    m_ioContext.run();
}

void http_n::server_n::Basic::setEndpoint(std::string method, const std::string& endpoint, handler_t handler) {
    if (not handler)
        throw InvalidArgument("No handler provided", "Endpoint handler");

    m_endpoints[method].addRoute(endpoint, handler);
}

void http_n::server_n::Basic::setStartSequence(const startSequence_t& function) {
    m_startSequence = function;
}

void http_n::server_n::Basic::setThreadPoolSize(uint16_t size) {
    m_threadPoolSize = size;
}

void http_n::server_n::Basic::startSequence() {
    if (not m_startSequence) return;

    m_light->log(log_n::Level_en::DEBUG, m_extraLogInformation, "Initiating starting sequence ...");
    m_startSequence(this);
}

void http_n::server_n::Basic::startThreadPool() {
    if (m_threadPoolSize > 0)
        m_light->log(log_n::Level_en::INFO, m_extraLogInformation, "Launching", m_threadPoolSize, "threads ...");

    for (uint16_t i = 0; i < m_threadPoolSize + BASE_THREADS; ++i) {
        m_threadIds.emplace_back(m_threadRegistry->start([&](){
            try {
                m_ioContext.run();
            } catch (std::exception& e) {
                m_light->log(log_n::Level_en::ERROR, m_extraLogInformation, "Exception in worker thread: ", e.what());
            }
        }).id());
    }
}

void http_n::server_n::Basic::stop() {
    if (not m_isRunning) return;

    m_light->log(log_n::Level_en::SPECIAL, m_extraLogInformation, "Stopping", "...");

    m_isRunning = false;
    m_workGuard.reset();
    m_ioContext.stop();
}