#pragma once

#include <system_error>
#include <iostream>

[[noreturn]] inline void throw_(const char *msg)
{
    throw   std::system_error(errno, std::generic_category(), msg);
}
