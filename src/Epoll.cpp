#include "Epoll.hpp"
#include "IPollable.hpp"
#include "Utils.hpp"

Epoll::Epoll() : _fd(-1) {}

Epoll::~Epoll()
{
	std::set<IPollable*>	destory;

	for (auto it = _objs.begin(); it != _objs.end(); ++it)
	{
		IPollable* obj = it->second;
		if (obj)
			destory.insert(obj);
	}
	for (std::set<IPollable*>::iterator it = destory.begin(); it != destory.end(); ++it)
	{
		IPollable* obj = *it;
		if (obj)
			obj->cleanup();
	}
	if (_fd >= 0)
		close(_fd);
}

Epoll&	Epoll::instance()
{
	static Epoll	inst;
	return (inst);
}

int	Epoll::init()
{
	_fd = epoll_create1(EPOLL_CLOEXEC);
	if (_fd < 0)
		throw_("epoll creation failed!");
	return (0);
}

int	Epoll::add_fd(IPollable* poll_obj, uint32_t events)
{
	if (_fd < 0)
		throw_("No epoll object is found!");
	int	obj_fd = poll_obj->fd();
	struct epoll_event	ev{};
	ev.events = events;
	ev.data.ptr = poll_obj;
	int status = epoll_ctl(_fd, EPOLL_CTL_ADD, obj_fd, &ev);
	if (status < 0)
		throw_("epoll_ctl ADD failed!");
	_objs[obj_fd] = poll_obj;
	return (status);
}

int Epoll::mod_fd(IPollable* poll_obj, uint32_t events)
{
	if (_fd < 0)
		throw_("No epoll object is found!");
	struct epoll_event	ev{};
	ev.events = events;
	ev.data.ptr = poll_obj;
	int status = epoll_ctl(_fd, EPOLL_CTL_MOD, poll_obj->fd(), &ev);
	if (status < 0)
		throw_("epoll_ctl MOD failed!");
	return (status);
}

int Epoll::del_fd(Fd fd_)
{
    if (_fd < 0 || fd_ < 0)
        return (-1);
    
	if (!_objs.count(fd_))
		throw_("No fd found!");
    _objs.erase(fd_);
    
   	int status = epoll_ctl(_fd, EPOLL_CTL_DEL, fd_, NULL);
    if (status < 0)
		throw_("epoll_ctl DELETE failed!");
    return (status);
}

int Epoll::wait(struct epoll_event *events, int maxevents, int timeout)
{
	if (_fd < 0)
		return (-1);
	return (epoll_wait(_fd, events, maxevents, timeout));
}

void Epoll::objs_timeout()
{
    std::vector<IPollable*> timed_out;
    std::set<IPollable*> seen;
    
    // Collect unique timed-out objects
    for (auto it = _objs.begin(); it != _objs.end(); ++it)
    {
        IPollable* obj = it->second;
        if (obj && obj->is_timeout())
        {
            if (seen.find(obj) == seen.end())
            {
                seen.insert(obj);
                timed_out.push_back(obj);
            }
        }
    }
    
    // Call timeout on collected objects
    for (size_t i = 0; i < timed_out.size(); ++i)
    {
        IPollable* obj = timed_out[i];
        if (obj)
            obj->timeout();
    }
}

int	Epoll::fd() const
{
	return (_fd);
}
