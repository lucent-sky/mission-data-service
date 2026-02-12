#include "http_server.h"

#include "validator.h"

#include <httplib.h>
#include <json.hpp>

#include <cstdlib>
#include <iostream>

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
    server_->Post("/ingest", [this](const httplib::Request& req,
                                     httplib::Response& res)
    {
        try
        {
            auto parsed = json::parse(req.body);

            if (!parsed.contains("source_id") ||
                !parsed.contains("value") ||
                !parsed.contains("timestamp"))
            {
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
                res.status = 422;
                json err = { {"error", error} };
                res.set_content(err.dump(), "application/json");
                return;
            }

            data_store_.add_record(validated.value());

            res.status = 200;
            res.set_content(R"({"status":"ok"})", "application/json");
        }
        catch (const json::exception&)
        {
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
    });

    // GET /stats
    server_->Get("/stats", [this](const httplib::Request&,
                                  httplib::Response& res)
    {
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
