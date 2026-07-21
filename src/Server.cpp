#include "Server.hpp"

Server::Server()
{
    _acceptor.create_socket("127.0.0.1", "8080");
    _reactor.add(_acceptor);
}

void    Server::run()
{
    _c_manager.set_reactor(_reactor);
    _acceptor.set_c_manager(_c_manager);
    _reactor.run();
}