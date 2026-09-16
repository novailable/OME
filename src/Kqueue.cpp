#include "Kqueue.hpp"
#include "IPollable.hpp"
#include "Utils.hpp"

Kqueue::Kqueue() : _fd(-1) {}

Kqueue::~Kqueue() {}

int Kqueue::init()
{
    _fd = kqueue();
    if (_fd < 0)
        throw_("kqueue creation failed!");
    return (0);
}

PollFlags	Kqueue::flags(int filter, uint16_t evflags)
{
	return static_cast<PollFlags>(
		  ((filter == EVFILT_READ)  ? READ : 0)
		| ((filter == EVFILT_WRITE) ? WRITE : 0)
		| ((evflags & EV_EOF)   ? HUP : 0)
		| ((evflags & EV_ERROR) ? ERR : 0));
}

int Kqueue::add_fd(IPollable* poll_obj, uint32_t flags)
{
    struct kevent changes[2];
    int n = 0;
    uint16_t clear = (flags & EDGE) ? EV_CLEAR : 0;

    if (flags & READ)
        EV_SET(&changes[n++], poll_obj->fd(), EVFILT_READ, EV_ADD | clear, 0, 0, poll_obj);
    if (flags & WRITE)
        EV_SET(&changes[n++], poll_obj->fd(), EVFILT_WRITE, EV_ADD | clear, 0, 0, poll_obj);

    return kevent(_fd, changes, n, nullptr, 0, nullptr);
}

int Kqueue::mod_fd(IPollable* poll_obj, uint32_t flags)
{
    return add_fd(poll_obj, flags);
}

int Kqueue::del_fd(IPollable* poll_obj)
{
    struct kevent changes[2];
    EV_SET(&changes[0], poll_obj->fd(), EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    EV_SET(&changes[1], poll_obj->fd(), EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
    kevent(_fd, changes, 2, nullptr, 0, nullptr); // ENOENT expected if only 1 filter was set
    return (0);
}

int Kqueue::wait(Events &ready, int timeout)
{
    static struct kevent raw[MAX_EVENTS];
    struct timespec ts{timeout / 1000, (timeout % 1000) * 1000000};

    int hits = kevent(_fd, nullptr, 0, raw, MAX_EVENTS, timeout >= 0 ? &ts : nullptr);
    for (int i = 0; i < hits; ++i)
		ready[i] = {static_cast<IPollable*>(raw[i].udata), flags(raw[i].filter, raw[i].flags)};
    return (hits);
}

int Kqueue::fd() const { return (_fd); }