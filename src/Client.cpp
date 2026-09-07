#include "Client.hpp"
#include "ClientManager.hpp"

Client::Client(int fd, sockaddr_in addr, ClientManager *manager) : _fd(fd), _addr(addr), _manager(manager)
{
    _ip = inet_ntoa(_addr.sin_addr);
    _port = ntohs(_addr.sin_port);

    _fd.set_nblock();
    // _time = time(NULL);
    _buffer.reserve(4096);
    std::cout << "[client]\tclient connected\t\t| " << _ip << ":" 
                << _port << " | socket:" << _fd << std::endl;

}

Client::~Client()  {}

void    Client::handle(uint32_t events)
{
    constexpr   size_t  CHUNK = 4096;
    std::cout << "client read" << std::endl;
    while(true)
    {
        size_t  pre_size = _buffer.size();
        _buffer.resize(pre_size + CHUNK);
        ssize_t bytes = recv(_fd, _buffer.data() + pre_size, CHUNK, 0);
        if (bytes > 0)
        {
            _buffer.resize(pre_size + static_cast<size_t>(bytes));
            continue;
        }
        _buffer.resize(pre_size);

        if (bytes == 0)
        {
            std::cout << "disconnected" << std::endl;
            cleanup();
            return ;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        cleanup();
        return ;
    }
    // Parser  parser
    std::cout << _buffer << std::endl;
}

void    Client::cleanup()
{
    std::cout << "[client]\tclient disconnected\t\t| " << _ip << ":" 
                << _port << " | socket:" << _fd << std::endl;
    _manager->remove(_fd);

}

int Client::fd() const
{
    return (_fd);
}

