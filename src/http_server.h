#pragma once

#include <memory>
#include <atomic>

#include "data_store.h"

namespace httplib
{
    class Server;
}

namespace mds
{

/**
 * @brief HTTP server wrapper for Mission Data Service.
 *
 * Responsible for:
 * - Endpoint registration
 * - Request routing
 * - Graceful shutdown
 */
class HttpServer
{
public:
    explicit HttpServer(DataStore& store);
    ~HttpServer();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;

    /**
     * @brief Starts listening on the specified port.
     *
     * Blocking call.
     */
    void start(int port);

    /**
     * @brief Stops the server gracefully.
     */
    void stop();

private:
    void configure_routes();

    DataStore& data_store_;
    std::unique_ptr<httplib::Server> server_;
    std::atomic<bool> running_{false};
};

} // namespace mds
