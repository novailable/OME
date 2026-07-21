#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#include "Fd.h"
#include "IPollable.hpp"

class Client : public IPollable
{
    private:
        Fd  _fd;
        std::string _ip;
        uint16_t    _port;
        sockaddr_in    _addr{};

        std::string _buffer;

    public:
        Client(int fd, sockaddr_in addr);
        ~Client();
        void    handle(uint32_t events);
        int fd() const;
        
};
