#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>

class   Server
{
    private:
        std::string _ip;
        int _fd;
        // std::map<int, Client> 
    public:
        Server();
        ~Server();
        void    run(std::string &ip, std::string port);
        void    end();
};
