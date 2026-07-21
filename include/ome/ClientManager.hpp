#pragma once

#include "Reactor.hpp"
#include "Client.hpp"

class   ClientManager
{
    private:
        Reactor&    _reactor;
        std::unordered_map<int, std::unique_ptr<Client>>    _sessions;
    public:
        ClientManager();
        
        // ClientManager(Reactor &reactor);
        void    set_reactor(Reactor& reactor);
        void    create(int fd, sockaddr_in addr);
};
