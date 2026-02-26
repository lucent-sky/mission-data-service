#pragma once

#include <memory>
#include <atomic>
#include <chrono>

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
    // Metrics measurements
    std::atomic<std::uint64_t> requests_total_{0};
    std::atomic<std::uint64_t> ingest_success_total_{0};
    std::atomic<std::uint64_t> ingest_validation_failures_total_{0};
    std::atomic<std::uint64_t> ingest_json_failures_total_{0};
    std::atomic<std::uint64_t> stats_requests_total_{0};

    std::atomic<std::uint64_t> total_latency_us_{0};

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
