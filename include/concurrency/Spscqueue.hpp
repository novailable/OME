#pragma once

#include <atomic>
#include <cstddef>
#include <array>

// Lock-free Single-Producer Single-Consumer ring buffer.
//
// Why no locks / no CAS loop is needed here:
//   - Exactly ONE thread ever calls push() -> it alone writes _head.
//   - Exactly ONE thread ever calls pop()  -> it alone writes _tail.
//   - Each thread only ever READS the other's index.
// Because there's no contention on who gets to write a given index,
// we never need compare_exchange. We only need to make sure that when
// the consumer sees an updated _head, it also sees the data the
// producer wrote *before* publishing that _head. That's what
// acquire/release give us:
//   - producer: write slot data, THEN release-store _head
//   - consumer: acquire-load _head, THEN read slot data
// release/acquire form a synchronizes-with pair: everything the
// producer did before the release-store is visible to the consumer
// after the acquire-load. A relaxed store would let the compiler/CPU
// reorder the data write after the index publish, so the consumer
// could read a slot before it's actually written.
template <typename T, size_t Capacity>
class	SPSCQueue
{
	private:
		// no actual work, kept purely to mark the point of "we're done
		// reading the slot" before publishing _tail -- the release store
		// on _tail already provides that ordering, this is just a label
		// for anyone reading the code.
		static void _head_consumed_fence() {}

		std::array<T, Capacity> _buffer;

		// alignas avoids false sharing: without it, _head and _tail could
		// land on the same cache line, and every push/pop would bounce
		// that line between the two cores' caches even though the two
		// threads never touch the same field.
		alignas(64) std::atomic<size_t> _head;
		alignas(64) std::atomic<size_t> _tail;

	public:
		static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");

		SPSCQueue() : _head(0), _tail(0) {}

		// called only by the producer thread
		bool push(const T& item)
		{
			const size_t head = _head.load(std::memory_order_relaxed);
			const size_t next = (head + 1) & (Capacity - 1);

			// acquire: we need to see the consumer's latest _tail so we
			// don't overwrite a slot it hasn't consumed yet.
			if (next == _tail.load(std::memory_order_acquire))
				return false; // queue full

			_buffer[head] = item;

			// release: publish the write above together with the new head.
			_head.store(next, std::memory_order_release);
			return true;
		}

		// called only by the consumer thread
		bool pop(T& out)
		{
			const size_t tail = _tail.load(std::memory_order_relaxed);

			// acquire: pairs with producer's release-store of _head, so
			// the item we're about to read is guaranteed visible.
			if (tail == _head.load(std::memory_order_acquire))
				return false; // queue empty

			out = _buffer[tail];

			const size_t next = (tail + 1) & (Capacity - 1);
			_head_consumed_fence(); // see note below (no-op, illustrative)
			_tail.store(next, std::memory_order_release);
			return true;
		}


};
