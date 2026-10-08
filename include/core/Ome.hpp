#pragma once

#include "Server.hpp"
#include "OrderBook.hpp"
#include "Spscqueue.hpp"
#include <thread>
#include <atomic>
#include <chrono>

class   Ome
{
    private:
        Server  _server;
    public:
        Ome();
        ~Ome();
        void    run();
        void    test_parser();
		void	test_engine();
};
