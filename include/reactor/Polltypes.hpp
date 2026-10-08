#pragma once

#include <cstdint>
#include <array>

struct IPollable;

constexpr	int MAX_EVENTS = 1024;

enum PollFlags : uint32_t
{
	NONE = 0,
	READ = 1u << 0,
	WRITE = 1u << 1,
	ERR = 1u << 2,
	HUP = 1u << 3,
	EDGE = 1u << 4,
};

inline PollFlags operator|(PollFlags a, PollFlags b)
{
    return static_cast<PollFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline PollFlags operator&(PollFlags a, PollFlags b)
{
    return static_cast<PollFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}
struct Event
{
	IPollable	*obj;
	PollFlags	flags;
};

using Events = std::array<Event, MAX_EVENTS>;