#pragma once

#include "Server.hpp"

class   Ome
{
    private:
        Server  _server;
    public:
        Ome();
        ~Ome();
        void    run();
        void    test_parser(std::string&);
};
