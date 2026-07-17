
// IPollable.hpp


#pragma once

#include <cstdint>

class IPollable
{
public:

    virtual ~IPollable() {}

    virtual int fd() const = 0;

    virtual void handle(uint32_t events) = 0;
};




//Fd.hpp


#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>


class Fd
{
private:

    int _fd;


public:

    explicit Fd(int fd = -1)
        : _fd(fd)
    {
    }


    ~Fd()
    {
        close();
    }


    Fd(const Fd&) = delete;
    Fd& operator=(const Fd&) = delete;


    int get() const
    {
        return _fd;
    }


    void close()
    {
        if (_fd >= 0)
        {
            ::close(_fd);
            _fd = -1;
        }
    }


    void set_nonblock()
    {
        int flags = fcntl(_fd, F_GETFL, 0);

        if (flags < 0)
            throw std::runtime_error("fcntl failed");

        if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) < 0)
            throw std::runtime_error("nonblock failed");
    }


    operator int() const
    {
        return _fd;
    }
};




//Epoll.hpp


#pragma once

#include <sys/epoll.h>


class Epoll
{

private:

    int _fd;


public:

    Epoll();

    ~Epoll();


    void add(IPollable* obj, uint32_t events);

    void modify(IPollable* obj, uint32_t events);

    void remove(IPollable* obj);


    int wait(
        epoll_event* events,
        int maxevents,
        int timeout
    );
};




//Epoll.cpp


#include "Epoll.hpp"
#include "IPollable.hpp"

#include <unistd.h>
#include <stdexcept>


Epoll::Epoll()
{
    _fd = epoll_create1(EPOLL_CLOEXEC);

    if (_fd < 0)
        throw std::runtime_error("epoll create failed");
}


Epoll::~Epoll()
{
    close(_fd);
}


void Epoll::add(IPollable* obj, uint32_t events)
{
    epoll_event ev{};

    ev.events = events;
    ev.data.ptr = obj;


    if (epoll_ctl(
            _fd,
            EPOLL_CTL_ADD,
            obj->fd(),
            &ev) < 0)
        throw std::runtime_error("epoll add failed");
}


void Epoll::modify(IPollable* obj, uint32_t events)
{
    epoll_event ev{};

    ev.events = events;
    ev.data.ptr = obj;


    if (epoll_ctl(
            _fd,
            EPOLL_CTL_MOD,
            obj->fd(),
            &ev) < 0)
        throw std::runtime_error("epoll mod failed");
}


void Epoll::remove(IPollable* obj)
{
    epoll_ctl(
        _fd,
        EPOLL_CTL_DEL,
        obj->fd(),
        nullptr
    );
}


int Epoll::wait(
    epoll_event* events,
    int maxevents,
    int timeout)
{
    return epoll_wait(
        _fd,
        events,
        maxevents,
        timeout
    );
}




//Reactor.hpp


#pragma once

#include "Epoll.hpp"


class Reactor
{

private:

    Epoll _epoll;

    bool _running;


public:

    Reactor();


    void add(IPollable* obj, uint32_t events);

    void remove(IPollable* obj);


    void run();

    void stop();
};




//Reactor.cpp


#include "Reactor.hpp"


Reactor::Reactor()
:
_running(true)
{
}


void Reactor::add(IPollable* obj, uint32_t events)
{
    _epoll.add(obj, events);
}


void Reactor::remove(IPollable* obj)
{
    _epoll.remove(obj);
}


void Reactor::run()
{
    epoll_event events[1024];


    while (_running)
    {

        int n = _epoll.wait(
            events,
            1024,
            1000
        );


        for (int i = 0; i < n; i++)
        {
            IPollable* obj =
                static_cast<IPollable*>(
                    events[i].data.ptr
                );


            obj->handle(
                events[i].events
            );
        }
    }
}


void Reactor::stop()
{
    _running = false;
}




//Acceptor.hpp


#pragma once

#include "IPollable.hpp"
#include "Fd.hpp"

#include <functional>


class SessionManager;


class Acceptor : public IPollable
{

private:

    Fd _fd;

    SessionManager& _sessions;


public:

    Acceptor(
        SessionManager& sessions
    );


    void listen(
        int port
    );


    int fd() const override;


    void handle(
        uint32_t events
    ) override;
};




//Acceptor.cpp


#include "Acceptor.hpp"
#include "SessionManager.hpp"


int Acceptor::fd() const
{
    return _fd.get();
}


void Acceptor::handle(uint32_t events)
{
    if (!(events & EPOLLIN))
        return;


    while(true)
    {
        int client =
            accept(
                _fd,
                nullptr,
                nullptr
            );


        if(client < 0)
        {
            if(errno == EAGAIN)
                break;

            throw std::runtime_error(
                "accept failed"
            );
        }


        _sessions.create(client);
    }
}




//Session.hpp


#pragma once

#include "IPollable.hpp"
#include "Fd.hpp"


class Session : public IPollable
{

private:

    Fd _fd;


public:

    Session(int fd);


    int fd() const override;


    void handle(
        uint32_t events
    ) override;
};




//Session.cpp


#include "Session.hpp"

#include <unistd.h>


Session::Session(int fd)
:
_fd(fd)
{
    _fd.set_nonblock();
}


int Session::fd() const
{
    return _fd.get();
}


void Session::handle(uint32_t events)
{

    if(events & EPOLLIN)
    {
        char buffer[4096];


        int n = read(
            _fd.get(),
            buffer,
            sizeof(buffer)
        );


        if(n <= 0)
        {
            // disconnect handling later
            return;
        }


        /*
            parser goes here

            message =
                Parser::parse(buffer)

            MatchingEngine::submit(message)
        */
    }
}




//SessionManager.hpp


#pragma once

#include <memory>
#include <unordered_map>


class Reactor;
class Session;


class SessionManager
{

private:

    Reactor& _reactor;


    std::unordered_map<
        int,
        std::unique_ptr<Session>
    > _sessions;


public:

    SessionManager(
        Reactor& reactor
    );


    void create(
        int fd
    );
};




//SessionManager.cpp


#include "SessionManager.hpp"

#include "Session.hpp"
#include "Reactor.hpp"


SessionManager::SessionManager(
    Reactor& reactor
)
:
_reactor(reactor)
{
}


void SessionManager::create(int fd)
{
    auto session =
        std::make_unique<Session>(fd);


    _reactor.add(
        session.get(),
        EPOLLIN | EPOLLET
    );


    _sessions.emplace(
        fd,
        std::move(session)
    );
}




//main.cpp


#include "Reactor.hpp"
#include "Acceptor.hpp"
#include "SessionManager.hpp"


int main()
{
    Reactor reactor;


    SessionManager sessions(
        reactor
    );


    Acceptor acceptor(
        sessions
    );


    acceptor.listen(8080);


    reactor.add(
        &acceptor,
        EPOLLIN | EPOLLET
    );


    reactor.run();
}


