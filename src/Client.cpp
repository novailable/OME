#include "Client.hpp"

Client::Client(int fd) : _fd(fd) {}

Client::handle(uint32_t events)
{

}

int Client::fd() const
{
    return (_fd);
}