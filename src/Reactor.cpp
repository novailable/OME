#include "Reactor.hpp"
#include "Utils.hpp"

Reactor::Reactor(Epoll& poller)
    : _poller(poller), _running(false)
{
}


Reactor::~Reactor()
{
    stop();
}


void Reactor::run()
{
    _running = true;

    epoll_event events[1024]{};

    while (_running)
    {
        int hits = _poller.wait(events, 1024, 1000);

        if (hits < 0)
        {
            if (errno == EINTR)
                continue;

            throw_("epoll wait failed");
        }


        for (int i = 0; i < hits; i++)
        {
            IPollable* obj =
                static_cast<IPollable*>(events[i].data.ptr);

            if (!obj)
                continue;

            obj->handle_event(events[i]);
        }


        // Handle timeout objects
        _poller.objs_timeout();
    }
}


void Reactor::stop()
{
    _running = false;
}