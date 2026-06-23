#include "Server.hpp"

Server::Server() : _fd(-1) {}

Server::~Server()
{
    end();
}

void    Server::run(std::string &ip, std::string port)
{
    addrinfo hints{};
    addrinfo *result = nullptr;

    _ip = ip;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int err = getaddrinfo(ip.empty() ? nullptr : 
        ip.c_str(), port.c_str(), &hints, &result);
    if (err != 0)
        throw   std::runtime_error(gai_strerror(err));
    try
    {
        _fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (_fd < 0)
            throw   std::runtime_error("socket() failed");
        int opt = 1;
        setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
        if (bind(_fd, result->ai_addr, result->ai_addrlen) < 0)
            throw   std::runtime_error("bind() failed");
        if (listen(_fd, SOMAXCONN) < 0)
            throw   std::runtime_error("listen() failed");
    }
    catch (...)
    {
        freeaddrinfo(result);
        throw;
    }
    freeaddrinfo(result);    
}

void    Server::end()
{
    if (_fd >= 0)
    {
        ::close(_fd);
        _fd = -1;
    }
}