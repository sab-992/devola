#include <asio.hpp>
#include <memory>
#include <thread>
#include <format>


class HttpFakeServer {
public:
    HttpFakeServer(HttpFakeServer& other) = delete;
    void operator=(const HttpFakeServer&) = delete;

    static std::shared_ptr<HttpFakeServer> getInstance() {
        if (m_instance.load() == nullptr)
            m_instance.store(std::shared_ptr<HttpFakeServer>(new HttpFakeServer())); 

        return m_instance.load();
    }

    uint16_t port() { return m_port; }

    void run() {
        if (m_thread and !m_stopped)
            return;

        m_thread = std::make_unique<std::jthread>(std::bind(&HttpFakeServer::start, this));
    }

    void stop() {
        m_stopped = true;
        if (!m_acceptor)
            return

        m_acceptor->close();
        m_acceptor = nullptr;

        m_thread->request_stop();
        m_thread->join();
    }

protected:
    HttpFakeServer() {}

private:
    static inline std::unique_ptr<asio::io_context> m_ioCtx = nullptr;
    static inline std::unique_ptr<asio::ip::tcp::acceptor> m_acceptor = nullptr;
    static inline std::atomic<std::shared_ptr<HttpFakeServer>> m_instance = nullptr;
    static inline uint16_t m_port = 4000;
    static inline bool m_stopped = false;
    static inline std::unique_ptr<std::jthread> m_thread = nullptr;

    size_t getContentLength(std::string headers) {
        int contentLengthValue = -1;
        std::string contentLengthKey = "Content-Length: ";
        size_t contentLengthIndex = headers.find(contentLengthKey);

        if (contentLengthIndex == std::string::npos)
            return contentLengthValue;

        size_t valueStart = contentLengthIndex + contentLengthKey.size();
        size_t lineEnd = headers.find("\r\n", valueStart);
        contentLengthValue = std::stoull(headers.substr(valueStart, lineEnd - valueStart));

        return contentLengthValue;
    }

    std::string readBody(asio::ip::tcp::socket& socket, std::string headers, size_t contentLength) {
        std::string endOfHeadersToken = "\r\n\r\n";
        std::string body = headers.substr(headers.find(endOfHeadersToken) + endOfHeadersToken.size());
        body.resize(contentLength);

        asio::read(socket, asio::dynamic_buffer(body), asio::transfer_exactly(contentLength - body.size()));
        return body;        
    }

    std::string readHeaders(asio::ip::tcp::socket& socket) {
        std::string headers;
        asio::read_until(socket, asio::dynamic_buffer(headers), "\r\n\r\n");
        return headers;
    }

    void start() {
        using namespace asio;

        m_stopped = false;
        m_ioCtx = std::make_unique<asio::io_context>();
        m_acceptor = std::make_unique<ip::tcp::acceptor>(*m_ioCtx, ip::tcp::endpoint(ip::tcp::v4(), m_port));

        while(!m_stopped) {
            asio::error_code ec;
            ip::tcp::socket socket(*m_ioCtx);
            m_acceptor->accept(socket, ec);

            if (ec) { break; } // stop() will cancel the accept(...). It will throw an asio::error_code to exit.

            std::string headers = readHeaders(socket);
            size_t contentLength = getContentLength(headers);

            std::string body;
            if (contentLength > 0)
                body = readBody(socket, headers, contentLength);
            else
                body = "Hello world!"; // Default body

            asio::write(socket, asio::buffer(std::format("HTTP/1.1 200 OK\r\nContent-Length: {}\r\n\r\n{}", body.size(), body)));
        }
    }
};