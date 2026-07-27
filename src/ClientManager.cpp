#include "ClientManager.hpp"

// ClientManager::ClientManager(Reactor &reactor) : _reactor(reactor) {}

ClientManager::ClientManager()
{
    
}

void    ClientManager::create(int fd, sockaddr_in addr)
{
    std::unique_ptr<Client> client = std::make_unique<Client>(fd, addr, this);
    _clients.emplace(fd, std::move(client));
    _reactor->add(_clients[fd].get(), EPOLLIN);
    std::cout << "client is added to epoll" << std::endl;
}

void    ClientManager::remove(int fd)
{
    auto    it = _clients.find(fd);
    if (it == _clients.end())
        return ;
    _reactor->del(it->second.get());
    _clients.erase(it);
    std::cout << "[manager]\tclient removed\t\t| socket:" << fd << std::endl;

}

void    ClientManager::set_reactor(Reactor& reactor)
{
    _reactor = &reactor;
}