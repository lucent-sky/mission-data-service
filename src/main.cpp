#include "http_server.h"
#include "data_store.h"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>

namespace
{
    std::unique_ptr<mds::HttpServer> g_server;

    void signal_handler(int)
    {
        if (g_server)
        {
            g_server->stop();
        }
    }

    int get_port_from_env()
    {
        const char* port_env = std::getenv("PORT");

        if (port_env == nullptr)
        {
            return 8080;
        }

        try
        {
            int port = std::stoi(port_env);
            return (port > 0 && port <= 65535) ? port : 8080;
        }
        catch (...)
        {
            return 8080;
        }
    }
}

int main()
{
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    const int port = get_port_from_env();

    mds::DataStore store;
    g_server = std::make_unique<mds::HttpServer>(store);

    g_server->start(port);

    return 0;
}
