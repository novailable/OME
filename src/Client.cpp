#include "Client.hpp"

Client::Client(int fd, sockaddr_in addr) : _fd(fd), _addr(addr)
{
    _ip = inet_ntoa(_addr.sin_addr);
    _port = ntohs(_addr.sin_port);

    _fd.set_nblock();
    _time = time(NULL);

    std::cout << "[connection]\tclient connected\t\t| " << _ip << ":" 
                << _port << " | socket:" << _fd << std::endl;

}

Client::handle(uint32_t events)
{

}

int Client::fd() const
{
    return (_fd);
}