#include <gtest/gtest.h>

#include <core/exception.h>
#include <format>
#include <core/http/http.h>
#include <core/http/request.h>
#include <core/process/registry.h>
#include <core/http/response.h>
#include <utils/core/inner_types.h>


using http_n::Request;
using http_n::Response;

template<typename T>
class HttpTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        if (m_processLauncher)
            mockServerProcess = std::make_unique<Process>(m_processLauncher->launch("python3", { std::format("{}/test/utils/core/mock_server.py", ROOT_DIRECTORY) }, true));
    }

    static void TearDownTestCase() {
        if (not mockServerProcess) return;

        mockServerProcess->terminate();
        mockServerProcess->wait();
    }

    http_n::Http getHttpObject(asio::io_context& ioCtx) {
        http_n::Http http(&ioCtx);
        http.disablePeerVerification();
        return std::move(http);
    }

    Request<T> createTestRequest() {
        return http_n::Request<T>().setMethod("GET")
                                   .setAPIEndpoint("/")
                                   .setURL("localhost")
                                   .setPort(8000).build();
    }

    asio::awaitable<Response<T>> asyncQueryMockServer(http_n::Http http, http_n::Request<T> request) {
        co_return co_await http.async_receive<T>(co_await http.async_send(request));
    }

private:
    inline static std::unique_ptr<Process> mockServerProcess;
    inline static std::unique_ptr<process_n::Registry> m_processLauncher = std::make_unique<process_n::Registry>();
};

TYPED_TEST_SUITE(HttpTest, networkInnerTypes_t);

TYPED_TEST(HttpTest, SynchronousOperationsWithValidRequest_ReturnsValidResponse) {
    asio::io_context ioCtx;
    http_n::Http http = this->getHttpObject(ioCtx);

    const Response<TypeParam>& response = http.receive<TypeParam>(http.send(this->createTestRequest()));

    EXPECT_EQ(response.status(), network_n::Code::OK);
}

TYPED_TEST(HttpTest, AsynchronousOperationsWithValidRequest_ReturnsValidResponse) {
    asio::io_context ioCtx;
    http_n::Http http = this->getHttpObject(ioCtx);

    http.spawn(this->asyncQueryMockServer(http, this->createTestRequest()),
               [](std::exception_ptr, Response<TypeParam> response) { EXPECT_EQ(response.status(), network_n::Code::OK); });

    ioCtx.run();
}

TYPED_TEST(HttpTest, Constructor_HasDefaultProtocol) {}