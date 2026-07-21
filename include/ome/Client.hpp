#pragma once

#include "Fd.h"
#include "IPollable.hpp"

class Client : public IPollable
{
    private:
        Fd  _fd;
        std::string _ip, _port;
        sockaddr    _addr{};

    public:
        Client(int fd, sockaddr addr);
        void    hanlde(uint32_t events);
        int fd() const;
        
};
