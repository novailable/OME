#include "ClientManager.hpp"

// ClientManager::ClientManager(Reactor &reactor) : _reactor(reactor) {}

ClientManager::ClientManager()
{
    
}

void    ClientManager::create(int fd, sockaddr_in addr)
{
    std::unique_ptr<Client> client = std::make_unique<Client>(fd, addr);
    _clients.emplace(fd, std::move(client));
    _reactor->add(_clients[fd].get(), EPOLLIN);
    
}

void    ClientManager::set_reactor(Reactor& reactor)
{
    _reactor = &reactor;
}