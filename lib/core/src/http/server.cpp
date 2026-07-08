#include <core/http/server.hpp>


http_n::server_n::Basic::Basic(const Private_s&, uint16_t port)
: m_ioContext(), m_workGuard(asio::make_work_guard(m_ioContext)), m_sslContext(asio::ssl::context::tls_server), m_port(port), m_signals(m_ioContext, SIGINT, SIGTERM), m_threadPoolSize(0) {
    setupCommands();

    m_light = log_n::Light::instance();
    m_threadRegistry = thread_n::Registry::instance();

    m_http = std::make_unique<Http>(&m_ioContext);
    m_sslContext.set_options(asio::ssl::context::no_sslv2 |
                             asio::ssl::context::no_sslv3);
    SSL_CTX_set_alpn_select_cb(m_sslContext.native_handle(), alpnSelectCallback, nullptr);
    std::string serverSettingsDir = std::format("{}/server/settings", ROOT_DIRECTORY);
    m_sslContext.use_certificate_chain_file(std::format("{}/server.crt", serverSettingsDir));
    m_sslContext.use_private_key_file(std::format("{}/server.key", serverSettingsDir), asio::ssl::context::pem);
    m_signals.async_wait([&](auto, auto){ stop(); });
    m_threadIds.reserve(THREADS_RESERVE_SIZE);
}

http_n::server_n::Basic::~Basic() {
    stop();
}

int http_n::server_n::Basic::alpnSelectCallback(SSL*, const unsigned char** out, unsigned char* outlen,
                                                      const unsigned char* in, unsigned int inlen, void* /*arg*/) {
    static const unsigned char supported[] = { 8, 'h','t','t','p','/','1','.','1' };
    int status = SSL_select_next_proto(const_cast<unsigned char**>(out), outlen, supported, sizeof(supported), in, inlen);

    if (status != OPENSSL_NPN_NEGOTIATED)
        return SSL_TLSEXT_ERR_ALERT_FATAL;

    return SSL_TLSEXT_ERR_OK;
}

std::unique_ptr<http_n::server_n::Basic> http_n::server_n::Basic::create(uint16_t port) {
    return std::make_unique<Basic>(Private_s(), port);
}

asio::awaitable<void> http_n::server_n::Basic::handleClient(asio::ip::tcp::socket&& socket) {
    using namespace http_n;
    try {
        Session session(asio::ssl::stream<asio::ip::tcp::socket>(std::move(socket), m_sslContext));

        co_await session.handshake();

        Request request;
        request.setProtocol(network_n::protocol_n::Factory::create(session.alpnExtension()));
        request.set(co_await session.read()).build();

        const std::string& method = request.method();
        const std::string& endpoint = request.APIEndpoint();

        if (not m_methodEndpoints.contains(method) or not m_methodEndpoints.at(method).contains(endpoint)) {
            co_await session.error(network_n::Code::NOT_FOUND);
            co_return;
        }

        const Response& response = m_methodEndpoints.at(method).at(endpoint)(session, request);
        co_await session.write(response);
    } catch (std::exception& e) {
        m_light->log(log_n::Level_en::ERROR, "Exception in worker thread. Function:", std::format("[{}]", FUNCTION_SIGNATURE), "Reason: ", e.what());
    }
}

asio::awaitable<void> http_n::server_n::Basic::listen() {
    using tcp = asio::ip::tcp;

    auto executor = co_await asio::this_coro::executor;
    tcp::acceptor acceptor(executor, { tcp::v4(), m_port });
    while(m_isRunning)
        m_http->spawn(handleClient(co_await acceptor.async_accept(asio::use_awaitable)), asio::detached);
}

void http_n::server_n::Basic::readCommands() const {
    m_light->log(log_n::Level_en::INFO, "Reading commands ...");
    while (m_isRunning) {
        std::string command;
        std::getline(std::cin, command);

        if (not m_commands.contains(command)) {
            m_light->log(log_n::Level_en::WARNING, "Command not found.");
            continue;
        }

        m_commands.at(command)();
    }
}

void http_n::server_n::Basic::run() {
    assert(m_methodEndpoints.size() > 0 && "No endpoints specified");

    m_isRunning = true;
    startSequence();

    startThreadPool();

    m_light->log(log_n::Level_en::INFO, "Listening at port:", m_port);
    m_http->spawn(listen(), asio::detached);

    readCommands();
}

void http_n::server_n::Basic::setEndpoints(std::string method, const endpointsUMap_t& endpoints) {
    if (endpoints.size() <= 0)
        throw InvalidArgument("No endpoints specified", "Endpoints umap");

    m_methodEndpoints[method] = endpoints;
}

void http_n::server_n::Basic::setStartSequence(const std::function<void()>& function) {
    m_startSequence = function;
}

void http_n::server_n::Basic::setThreadPoolSize(uint16_t size) {
    m_threadPoolSize = size;
}

void http_n::server_n::Basic::setupCommands() {
    m_commands["stop"] = std::bind(&Basic::stop, this);
}

void http_n::server_n::Basic::startSequence() const {
    if (not m_startSequence) return;

    m_light->log(log_n::Level_en::INFO, "Initiating starting sequence ...");
    m_startSequence();
}

void http_n::server_n::Basic::startThreadPool() {
    if (m_threadPoolSize > 0)
        m_light->log(log_n::Level_en::INFO, "Initiating", m_threadPoolSize, "threads ...");

    for (uint16_t i = 0; i < m_threadPoolSize + BASE_THREADS; i++) {
        m_threadIds.emplace_back(m_threadRegistry->start([&](){
            try {
                m_ioContext.run();
            } catch (std::exception& e) {
                m_light->log(log_n::Level_en::ERROR, "Exception in worker thread: ", e.what());
            }
        }).id());
    }
}

void http_n::server_n::Basic::stop() {
    if (not m_isRunning) return;

    m_isRunning = false;
    m_workGuard.reset();
    m_ioContext.stop();

    for (const auto& id : m_threadIds)
        m_threadRegistry->join(id);
}