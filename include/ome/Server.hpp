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

class   Server
{
    private:
        std::string _ip;
        int _srvfd, _epfd;
        std::set<int>   clients;
        void    epoll_init();
        std::unordered_map<int, std::string>    _clients;

    public:
        Server();
        ~Server();
        void    run(std::string &ip, std::string port);
        void    end();
        void    epoll_add(int fd, uint32_t events);
        void    epoll_mod(int fd, uint32_t events);
        void    epoll_del(int fd);
        static void    set_nblock(int fd);
};
