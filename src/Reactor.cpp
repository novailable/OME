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

void    Reactor::add(IPollable* poll_obj, uint32_t events)
{
    _poller.add_fd(poll_obj, events);
}


void Reactor::run()
{
    _running = true;

    epoll_event events[1024]{};

    while (_running)
    {
        if (_poller.wait(events, 1024, 1000) < 0)
        {
            if (errno == EINTR)
                continue;

            throw_("epoll wait failed");
        }
        for (auto event : events)
        {
            IPollable* obj =
                static_cast<IPollable*>(event.data.ptr);

            if (!obj)
                continue;

            obj->handle(event.events);        
        }
        // Handle timeout objects
        // _poller.objs_timeout();
    }
}


void Reactor::stop()
{
    _running = false;
}