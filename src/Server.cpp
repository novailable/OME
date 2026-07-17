#include "Server.hpp"
#include "Utils.hpp"

Server::Server() : _fd(-1) {}

Server::~Server()
{
    // for(auto& [fd, buffer] : _clients)
    // {
    //     epoll_del(fd);
    //     ::close(fd);
    // }
    // _clients.clear();
    // end();
}
void    Server::create_socket(std::string &ip, std::string port)
{
    _ip = ip;
    _port = port;
    addrinfo hints{};
    addrinfo *result = nullptr;
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int err = getaddrinfo(ip.empty() ? nullptr : 
        ip.c_str(), _port.c_str(), &hints, &result);
    if (err != 0)
        throw   std::runtime_error(gai_strerror(err));
    try
    {
        _fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (_fd < 0)
            throw_("server socket failed");
        int opt = 1;
        setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
        if (bind(_fd, result->ai_addr, result->ai_addrlen) < 0)
            throw_("bind() failed");
        if (listen(_fd, SOMAXCONN) < 0)
            throw_("listen() failed");
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

int Server::fd() const
{
    return (_fd);
}

void    Server::handle()
{
    int client_fd = -1;

    if (!events & EPOLLIN)
        return ;
    while(true)
    {
        sockaddr_in client_addr{};
        client_fd = accept(_fd, (sockaddr *)&client_addr, &sizeof(client_addr));

        if (client_fd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return;

            fail("Connection: Accept", errno);
            return;
        }
        ClientManager   
        // Get client info
        _ip = inet_ntoa(client_addr.sin_addr);
        _port = ntohs(client_addr.sin_port);

        // Set non-blocking
        int flags = fcntl(_fd, F_GETFL, 0);
        if (flags < 0)
        {
            fail("Connection: F_GETFL", errno);
            close(_fd);
            _fd = -1;
            return;
        }

        if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) < 0)
        {
            fail("Connection: F_SETFL", errno);
            close(_fd);
            _fd = -1;
            return;
        }

        _time = time(NULL);

        std::cout << "[connection]\tclient connected\t\t| " << _ip << ":" 
                    << _port << " | socket:" << _fd << std::endl;

            int client = accpet
    }
}