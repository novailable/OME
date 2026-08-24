#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#include "Fd.h"
#include "IPollable.hpp"

class ClientManager;

class Client : public IPollable
{
    private:
        Fd  _fd;
        std::string _ip;
        uint16_t    _port;
        sockaddr_in    _addr{};
        ClientManager  *_manager;
        
        
        std::string _buffer;

    public:
        Client(int fd, sockaddr_in addr, ClientManager *manager);
        ~Client();
        void    handle(uint32_t events);
        void    cleanup();
        int fd() const;

        std::string_view    data() const;
        void    remove(size_t n);
};
