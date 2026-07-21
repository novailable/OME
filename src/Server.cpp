#include "Server.hpp"

Server::Server()
{
    std::string ip = "127.0.0.1", port = "8080";
    _acceptor.create_socket(ip, port);
    std::cout << "Server - " << ip << ":" << port << std::endl; 
    _reactor.add(&_acceptor, EPOLLIN | EPOLLET);
    std::cout << "Server is added to epoll" << std::endl;
}

void    Server::run()
{
    _c_manager.set_reactor(_reactor);
    _acceptor.set_c_manager(_c_manager);
    _reactor.run();
}