#include "Epoll.hpp"
#include "IPollable.hpp"
#include "Utils.hpp"

Epoll::Epoll() : _fd(-1) {}

Epoll::~Epoll()
{
	// std::set<IPollable*>	destory;

	// for (auto it = _objs.begin(); it != _objs.end(); ++it)
	// {
	// 	IPollable* obj = it->second;
	// 	if (obj)
	// 		destory.insert(obj);
	// }
	// for (std::set<IPollable*>::iterator it = destory.begin(); it != destory.end(); ++it)
	// {
	// 	IPollable* obj = *it;
	// 	if (obj)
	// 		obj->cleanup();
	// }
}

// Epoll&	Epoll::instance()
// {
// 	static Epoll	inst;
// 	return (inst);
// }

int	Epoll::init()
{
	
	_fd = epoll_create1(EPOLL_CLOEXEC);
	if (_fd < 0)
		throw_("epoll creation failed!");
	return (0);
}

uint32_t	Epoll::flags(PollFlags flags)
{
	return ((flags & READ ? EPOLLIN : 0)
			| (flags & WRITE ? EPOLLOUT : 0)
			| (flags & ERR ? EPOLLERR : 0)
			| (flags & HUP ? EPOLLHUP : 0))
			| (flags & EDGE ? EPOLLLET : 0);
}

Pollflag	Epoll::flags(uint32_t events)
{
	return (static_cast<PollFlags>
			((events & EPOLLIN  ? READ  : 0)
        	| (events & EPOLLOUT ? WRITE : 0)
			| (events & EPOLLERR ? ERROR : 0)
			| (events & EPOLLHUP ? POLL_HUP : 0)));
}

int	Epoll::add_fd(IPollable* poll_obj, PollFlags events)
{
	if (_fd < 0)
		throw_("No epoll object is found!");

	struct epoll_event	ev{};
	ev.events = flags(events);
	ev.data.ptr = poll_obj;

	int status = epoll_ctl(_fd, EPOLL_CTL_ADD, poll_obj->fd(), &ev);
	if (status < 0)
		throw_("epoll_ctl ADD failed!");
	// _objs[obj_fd] = poll_obj;

	return (status);
}

int Epoll::mod_fd(IPollable* poll_obj, PollFlags events)
{
	if (_fd < 0)
		throw_("No epoll object is found!");
	struct epoll_event	ev{};
	ev.events = flags(events);
	ev.data.ptr = poll_obj;
	int status = epoll_ctl(_fd, EPOLL_CTL_MOD, poll_obj->fd(), &ev);
	if (status < 0)
		throw_("epoll_ctl MOD failed!");
	return (status);
}

int Epoll::del_fd(IPollable* poll_obj)
{
    int	obj_fd = poll_obj->fd();
   	int status = epoll_ctl(_fd, EPOLL_CTL_DEL, obj_fd, NULL);
    if (status < 0)
		throw_("epoll_ctl DELETE failed!");
    return (status);
}

int Epoll::wait(Events &ready, int timeout)
{
	if (_fd < 0)
		return (-1);
	static struct epoll_event raw[1024]{};
	int hits = epoll_wait(_fd, raw, MAX_EVENTS, timeout);
	for(int i = 0; i < hits; ++i)
	{
		ready[i] = {static_cast<IPollable*>(raw[i].data.ptr), flags(raw[i].events)};
	}
	return (hits);
}



// void Epoll::objs_timeout()
// {
//     std::vector<IPollable*> timed_out;
//     std::set<IPollable*> seen;
    
//     // Collect unique timed-out objects
//     for (auto it = _objs.begin(); it != _objs.end(); ++it)
//     {
//         IPollable* obj = it->second;
//         if (obj && obj->is_timeout())
//         {
//             if (seen.find(obj) == seen.end())
//             {
//                 seen.insert(obj);
//                 timed_out.push_back(obj);
//             }
//         }
//     }
    
//     // Call timeout on collected objects
//     for (size_t i = 0; i < timed_out.size(); ++i)
//     {
//         IPollable* obj = timed_out[i];
//         if (obj)
//             obj->timeout();
//     }
// }

int	Epoll::fd() const
{
	return (_fd);
}
