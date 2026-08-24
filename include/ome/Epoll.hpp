#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <set>
#include <vector>
#include <map>

#include "Fd.h"
#include "IPollable.hpp"

struct IPollable; // forward

struct Epoll
{
	private: 
		Fd	_fd;
	public:
		Epoll();
		~Epoll();
		static Epoll& instance();

		int init();

		operator	Fd() const;

		int add_fd(IPollable* poll_obj,uint32_t events);
		int mod_fd(IPollable* poll_obj, uint32_t events);
		int del_fd(IPollable* poll_obj);

		int wait(struct epoll_event *events, int maxevents, int timeout);
		// void	objs_timeout();

		int	fd() const;
};

#endif
