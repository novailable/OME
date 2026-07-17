#pragma once

#include "Reactor.hpp"
#include "Client.hpp"

class   ClientManager
{
    private:
        Reactor&    _reactor;
        std::unordered_map<int, std::unique_ptr<Client>>    _sessions;
    public:
        ClientManager(Reactor &reactor);
        void    create(int fd);
};
