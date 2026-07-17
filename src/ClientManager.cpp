#include "ClientManager.hpp"

ClientManager::ClientManager(Reactor &reactor) : _reactor(reactor) {}

void    ClientManager::create(int fd)
{
    auto    client = std::make_unique<Client>(fd);
}