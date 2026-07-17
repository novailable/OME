#pragma once

#include "Fd.h"
#include "IPollable.hpp"

class Client : public IPollable
{
    private:
        Fd  _fd = -1;
    public:
        Client(int fd);
        void    hanlde(uint32_t events);
        int fd() const;
        
}