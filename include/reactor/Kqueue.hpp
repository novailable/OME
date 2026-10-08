#pragma once

#include <sys/event.h>
#include <sys/types.h>
#include <unistd.h>

#include "Fd.h"
#include "IPollable.hpp"
#include "Polltypes.hpp"

struct IPollable;

struct Kqueue
{
    private:
        Fd _fd;
    public:
        Kqueue();
        ~Kqueue();
        int init();
		PollFlags	flags(int filter, uint16_t evflags);
        int add_fd(IPollable* poll_obj, uint32_t flags);
        int mod_fd(IPollable* poll_obj, uint32_t flags);
        int del_fd(IPollable* poll_obj);
        int wait(Events &ready, int timeout);
        int fd() const;
};