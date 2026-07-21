#include "Server.hpp"
#include "Utils.hpp"

Acceptor::Acceptor() : _fd(-1) , _c_manager(nullptr) {}

Acceptor::~Acceptor()
{
    // for(auto& [fd, buffer] : _clients)
    // {
    //     epoll_del(fd);
    //     ::close(fd);
    // }
    // _clients.clear();
    // end();
}

void    Acceptor::set_c_manager(ClientManager &c_manager)
{
    _c_manager = &c_manager;
}

void    Acceptor::create_socket(std::string &ip, std::string port)
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
        _fd.set_nblock();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        freeaddrinfo(result);
        throw;
    }
    freeaddrinfo(result);
}



void    Acceptor::end()
{
    _fd.close();
}

int Acceptor::fd() const
{
    return (_fd);
}

void    Acceptor::handle(uint32_t events)
{
    int client_fd = -1;

    if (!(events & EPOLLIN))
        return ;
    while(true)
    {
        sockaddr_in client_addr{};
        socklen_t   client_len = sizeof(client_addr);
        client_fd = accept(_fd, (sockaddr *)&client_addr, &client_len);

        if (client_fd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return;

            throw_("Connection: Accept");
            return;
        }
        _c_manager->create(client_fd, client_addr);
    }
}