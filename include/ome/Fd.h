#pragma once

#include <fcntl.h>
#include "Utils.hpp"

class Fd
{
    private:
        int _fd;
        
    public:
        explicit Fd(int fd = -1) : _fd(fd) {};

        Fd(const Fd&) = delete;
        Fd  &operator=(const Fd&) = delete;
        Fd  &operator=(int fd)
        {
            close();
            _fd = fd;
            return (*this);
        }
        ~Fd() { close(); }

        int get() const { return (_fd); }
        void    close()
        {
            if (_fd >= 0)
                if (::close(_fd) < 0)
                    throw_("fd closing failed");
            _fd = -1;
        };

        void    set_nblock()
        {
            if (_fd < 0)
                throw_("none file descriptor to set non-blocking");
            if (fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL, 0) | O_NONBLOCK) < 0)
                throw_("non-blocking set failed");
        }

        operator int() const {return _fd;}
};
