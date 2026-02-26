#include "http_server.h"

#include "validator.h"

#include <httplib.h>
#include <json.hpp>

#include <cstdlib>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

namespace mds
{

HttpServer::HttpServer(DataStore& store)
    : data_store_(store),
      server_(std::make_unique<httplib::Server>())
{
    configure_routes();
}

HttpServer::~HttpServer()
{
    stop();
}

void HttpServer::configure_routes()
{
    // POST /ingest
    server_->Post("/ingest", [this](const httplib::Request& req, httplib::Response& res)
    {
        requests_total_.fetch_add(1, std::memory_order_relaxed);

        const auto start = std::chrono::high_resolution_clock::now();
        try
        {
            auto parsed = json::parse(req.body);

            if (!parsed.contains("source_id") ||
                !parsed.contains("value") ||
                !parsed.contains("timestamp"))
            {
                ingest_validation_failures_total_.fetch_add(1, std::memory_order_relaxed);
                res.status = 400;
                res.set_content(R"({"error":"Missing required fields"})",
                                "application/json");
                return;
            }

            const std::string source_id = parsed.at("source_id").get<std::string>();
            const double value = parsed.at("value").get<double>();
            const std::uint64_t timestamp = parsed.at("timestamp").get<std::uint64_t>();

            std::string error;
            auto validated = Validator::validate(source_id, value, timestamp, error);

            if (!validated.has_value())
            {
                ingest_validation_failures_total_.fetch_add(1, std::memory_order_relaxed);
                res.status = 422;
                json err = { {"error", error} };
                res.set_content(err.dump(), "application/json");
                return;
            }

            data_store_.add_record(validated.value());

            ingest_success_total_.fetch_add(1, std::memory_order_relaxed);

            res.status = 200;
            res.set_content(R"({"status":"ok"})", "application/json");
        }
        catch (const json::exception&)
        {
            ingest_json_failures_total_.fetch_add(1, std::memory_order_relaxed);
            res.status = 400;
            res.set_content(R"({"error":"Malformed JSON"})",
                            "application/json");
        }
        catch (...)
        {
            res.status = 500;
            res.set_content(R"({"error":"Internal server error"})",
                            "application/json");
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        total_latency_us_.fetch_add(static_cast<std::uint64_t>(duration),
                                    std::memory_order_relaxed);
    });

    // GET /stats
    server_->Get("/stats", [this](const httplib::Request&,
                                  httplib::Response& res)
    {
        requests_total_.fetch_add(1, std::memory_order_relaxed);
        stats_requests_total_.fetch_add(1, std::memory_order_relaxed);

        auto stats = data_store_.compute_stats();

        if (!stats.has_value())
        {
            res.status = 204;
            return;
        }

        json response = {
            {"count", stats->count},
            {"min", stats->min},
            {"max", stats->max},
            {"average", stats->average}
        };

        res.status = 200;
        res.set_content(response.dump(), "application/json");
    });

    server_->Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"healthy\"}", "application/json");
    });

    server_->Get("/metrics", [this](const httplib::Request&,
                                    httplib::Response& res)
    {
        const auto total_requests = requests_total_.load(std::memory_order_relaxed);
        const auto success = ingest_success_total_.load(std::memory_order_relaxed);
        const auto validation_fail =
            ingest_validation_failures_total_.load(std::memory_order_relaxed);
        const auto json_fail =
            ingest_json_failures_total_.load(std::memory_order_relaxed);
        const auto stats_calls =
            stats_requests_total_.load(std::memory_order_relaxed);
        const auto total_latency =
            total_latency_us_.load(std::memory_order_relaxed);

        double avg_latency_ms = 0.0;
        if (total_requests > 0)
        {
            avg_latency_ms =
                (static_cast<double>(total_latency) / total_requests) / 1000.0;
        }

        std::ostringstream metrics;

        metrics << "mission_requests_total " << total_requests << "\n";
        metrics << "mission_ingest_success_total " << success << "\n";
        metrics << "mission_ingest_validation_failures_total "
                << validation_fail << "\n";
        metrics << "mission_ingest_json_failures_total "
                << json_fail << "\n";
        metrics << "mission_stats_requests_total "
                << stats_calls << "\n";
        metrics << "mission_avg_latency_ms "
                << avg_latency_ms << "\n";

        res.set_content(metrics.str(), "text/plain");
    });
}

void HttpServer::start(int port)
{
    running_.store(true);
    std::cout << "Mission Data Service starting on port "
              << port << std::endl;

    server_->listen("0.0.0.0", port);
}

void HttpServer::stop()
{
    if (running_.exchange(false))
    {
        server_->stop();
        std::cout << "Mission Data Service stopped." << std::endl;
    }
}

} // namespace mds