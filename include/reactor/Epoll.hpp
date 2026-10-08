#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>

#include "Fd.h"
#include "Polltypes.hpp"
#include "IPollable.hpp"

struct IPollable; // forward

struct Epoll
{
	private: 
		Fd	_fd;
		static uint32_t		flags(PollFlags flags);
		static PollFlags	flags(uint32_t events);

	public:
		Epoll();
		~Epoll();
		// static Epoll& instance();

		int init();

		int add_fd(IPollable* poll_obj,PollFlags events);
		int mod_fd(IPollable* poll_obj, PollFlags events);
		int del_fd(IPollable* poll_obj);

		int wait(Events &ready, int timeout);
		// void	objs_timeout();

		int	fd() const;
};

#endif
