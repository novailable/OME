#ifndef IPOLLABLE_HPP
#define IPOLLABLE_HPP

#include "Utils.hpp"
#include "Fd.h"

struct	IPollable
{
	virtual	~IPollable() {};
	virtual	int fd() const = 0;
	virtual void	handle(uint32_t) = 0;
	virtual bool	is_timeout() const {return (false);}
	virtual void	timeout() {}
	virtual void	cleanup() {}
};

#endif
