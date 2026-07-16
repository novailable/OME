#pragma once

#include "Epoll.hpp"
#include "IPollable.hpp"

#include <sys/epoll.h>
#include <atomic>

class Reactor
{
private:
    Epoll&              _poller;
    std::atomic<bool>   _running;

public:
    Reactor(Epoll& poller);
    ~Reactor();

    void run();
    void stop();
};
