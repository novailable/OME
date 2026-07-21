#include "Ome.hpp"

void    Ome::run()
{
    Acceptor  server;
    server.create_socket("127.0.0.1", "8080");

    Epoll   poller;
    poller.init();
    poller.add_fd(server, EPOLLIN)
    Reactor reactor(poller);
    reactor.run()
}