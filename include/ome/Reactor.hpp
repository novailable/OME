#pragma once

#include "Epoll.hpp"
#include "IPollable.hpp"
#include "Utils.hpp"

#include <sys/epoll.h>
#include <atomic>

class Reactor
{
private:
    Epoll              _poller;
    std::atomic<bool>   _running;

public:
    Reactor();
    ~Reactor();

    void run();
    void add(IPollable* poll_obj, uint32_t events);
    void stop();
};
