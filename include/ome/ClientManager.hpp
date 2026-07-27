#pragma once

#include <unordered_map>
#include <memory>

#include "Reactor.hpp"
#include "Client.hpp"

class Client;

class   ClientManager
{
    private:
        Reactor    *_reactor;
        std::unordered_map<int, std::unique_ptr<Client>>    _clients;
    public:
        ClientManager();
        
        // ClientManager(Reactor &reactor);
        void    set_reactor(Reactor& reactor);
        void    create(int fd, sockaddr_in addr);
        void    remove(int fd);
};
