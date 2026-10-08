#include "Reactor.hpp"
#include "Utils.hpp"

Reactor::Reactor() : _running(false)
{
    _poller.init();
    std::cout << "epoll initialized" << std::endl;
}


Reactor::~Reactor()
{
    stop();
}

void    Reactor::add(IPollable* poll_obj, PollFlags events)
{
    _poller.add_fd(poll_obj, events);
}

void    Reactor::del(IPollable* poll_obj)
{
    _poller.del_fd(poll_obj);
}

void Reactor::run()
{
    _running = true;

	Events	ready{};
    while (_running)
    {
        int hits = _poller.wait(ready, 1000);
        if (hits < 0)
        {
            if (errno == EINTR)
                continue;

            throw_("epoll wait failed");
        }
        for (int i = 0; i < hits; ++i)
        {
            if (!ready[i].obj)
                continue;
            ready[i].obj->handle(ready[i].flags);        
        }
        // Handle timeout objects
        // _poller.objs_timeout();
    }
}


void Reactor::stop()
{
    _running = false;
}