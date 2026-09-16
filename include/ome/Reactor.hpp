#pragma once

#if defined(__linux__)
	#include "Epoll.hpp"
	using PlatformPoller = Epoll;
#elif defined(__APPLE__)
	#include "Kqueue.hpp"
	using PlatformPoller = Kqueue;
#endif

#include "IPollable.hpp"
#include "Utils.hpp"

#include <queue>
#include <atomic>

class Reactor
{
private:
    PlatformPoller		_poller;
    std::atomic<bool>   _running;

public:
    Reactor();
    ~Reactor();

    void run();
    void add(IPollable* poll_obj, PollFlags events);
    void del(IPollable* poll_obj);
    void stop();
};
