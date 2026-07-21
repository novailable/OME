#pragma once

#include "Acceptor.hpp"
#include "Reactor.hpp"
#include "ClientManager.hpp"

class Server
{
    private:
        Acceptor    _acceptor;
        Reactor     _reactor;
        ClientManager   _c_manager;

    public:
        Server();
        void    run();
};
