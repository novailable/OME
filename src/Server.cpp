#include "Server.hpp"
#include "Utils.hpp"

Server::Server() : _srvfd(-1), _epfd(-1) {}

Server::~Server()
{
    for(auto& [fd, buffer] : _clients)
    {
        epoll_del(fd);
        ::close(fd);
    }
    _clients.clear();
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
        _srvfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (_srvfd < 0)
            throw_("server socket failed");
        int opt = 1;
        setsockopt(_srvfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
        if (bind(_srvfd, result->ai_addr, result->ai_addrlen) < 0)
            throw_("bind() failed");
        if (listen(_srvfd, SOMAXCONN) < 0)
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
    if (_srvfd >= 0)
    {
        ::close(_srvfd);
        _srvfd = -1;
    }
}

void    Server::epoll_add(int fd, uint32_t events)
{
    epoll_event ev{};
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) < 0)
        throw_("epoll_ctl ADD failed");
}

void    Server::epoll_del(int fd)
{
    if (epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL) < 0)
        throw_("epoll_ctl DELETE failed");
}

void    Server::epoll_mod(int fd, uint32_t events)
{
    epoll_event ev{};
    ev.events = events;
    if (epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev))
        throw_("epoll_ctl MOD failed");
}

void Server::set_nblock(int fd)
{
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) < 0)
        throw_("non-blocking set failed");
}

void    Server::epoll_init()
{
    _epfd = epoll_create1(EPOLL_CLOEXEC);
    if (_epfd < 0)
        throw_("epoll creation failed!");
    epoll_add(_srvfd, EPOLLIN);

    epoll_event events[1024]{};
    while (1)
    {
        int hits = epoll_wait(_epfd, events, 1024, 1000);
        if (hits < 0)
        {   
            if (errno == EINTR)
                continue;
            throw_("epoll wait failed");
        }
        for (int i = 0; i < hits; ++i)
        {
            if (events[i].data.fd == _srvfd)
            {
                int             fd;
                sockaddr_in     cli_addr{};
                socklen_t       cli_len = sizeof(cli_addr);
                fd = accept(_srvfd, (sockaddr *)&cli_addr, &(cli_len));
                try
                {
                    if (fd < 0)
                    {
                        if (errno == EWOULDBLOCK || errno == EAGAIN)
                            continue;
                        throw_("clients accept failed");
                    }
                    set_nblock(fd);
                    epoll_add(fd, EPOLLIN);
                    _clients[fd] = "";
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
        }
    }
    epoll_del(_srvfd);
}