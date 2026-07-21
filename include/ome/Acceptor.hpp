#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <set>
#include <unordered_map>

#include "Fd.h"
#include "IPollable.hpp"
#include "ClientManager.hpp"

class   Acceptor : public IPollable
{
    private:
        std::string _ip, _port;
        Fd _fd;
        ClientManager   &_c_manager = nullptr;


    public:
        Acceptor();
        ~Acceptor();
        void    set_c_manager(ClientManager &c_manager);
        void    create_socket(std::string &ip, std::string port);
        void    handle(uint32_t);
        // bool    is_timeout() const;
        // void    timeout();
        // void    cleanup();
        void    end();
        int     fd() const;
};
