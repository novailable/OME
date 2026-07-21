#include "Client.hpp"

Client::Client(int fd, sockaddr_in addr) : _fd(fd), _addr(addr)
{
    _ip = inet_ntoa(_addr.sin_addr);
    _port = ntohs(_addr.sin_port);

    _fd.set_nblock();
    // _time = time(NULL);

    std::cout << "[client]\tclient connected\t\t| " << _ip << ":" 
                << _port << " | socket:" << _fd << std::endl;

}
Client::~Client()
{
    std::cout << "client quit" << std::endl;
}

void    Client::handle(uint32_t events)
{
    char    buffer[4096];

    while(true)
    {
        ssize_t bytes = recv(_fd, buffer, sizeof(buffer), 0);
        if (bytes > 0)
        {
            _buffer.append(buffer, bytes);
            continue;
        }
        else if (bytes == 0)
        {
            delete this;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
    }
}

int Client::fd() const
{
    return (_fd);
}

