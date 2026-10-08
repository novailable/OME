#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

template <typename T, std::size_t Capacity>
class SPSCQueue
{
	private:
		static_assert(Capacity > 1 && (Capacity & (Capacity - 1)) == 0,
					"Capacity must be a power of 2 and greater than 1");

		// Raw memory capable of holding a T.
		// No T objects are constructed here.
		using Storage = std::aligned_storage_t<sizeof(T), alignof(T)>;

		std::array<Storage, Capacity> _buffer;

		alignas(64) std::atomic<std::size_t> _head{0};

		alignas(64) std::atomic<std::size_t> _tail{0};

		static constexpr std::size_t mask = Capacity - 1;

		T* ptr(std::size_t index) noexcept
		{
			return std::launder(reinterpret_cast<T*>(&_buffer[index]));
		}

	public:
		SPSCQueue() noexcept = default;

		~SPSCQueue()
		{
			// Destroy objects that are still inside the queue.
			std::size_t tail = _tail.load(std::memory_order_relaxed);
			const std::size_t head = _head.load(std::memory_order_relaxed);

			while (tail != head)
			{
				std::destroy_at(ptr(tail));
				tail = (tail + 1) & mask;
			}
		}

		bool push(const T& item)
		{
			const std::size_t head = _head.load(std::memory_order_relaxed);
			const std::size_t next = (head + 1) & mask;

			if (next == _tail.load(std::memory_order_acquire))
				return false;

			::new (static_cast<void*>(ptr(head))) T(item);
			_head.store(next, std::memory_order_release);

			return true;
		}

		bool push(T&& item)
		{
			const std::size_t head = _head.load(std::memory_order_relaxed);

			const std::size_t next = (head + 1) & mask;

			if (next == _tail.load(std::memory_order_acquire))
				return false;

			// Move-construct directly into queue slot.
			::new (static_cast<void*>(ptr(head))) T(std::move(item));
			_head.store(next, std::memory_order_release);

			return true;
		}

		bool pop(T& out)
		{
			const std::size_t tail = _tail.load(std::memory_order_relaxed);

			if (tail == _head.load(std::memory_order_acquire))
				return false;

			T* item = ptr(tail);
			out = std::move(*item);
			item->~T();

			const std::size_t next = (tail + 1) & mask;

			_tail.store(next, std::memory_order_release);

			return true;
		}

		bool empty() const noexcept
		{
			const auto tail = _tail.load(std::memory_order_acquire);
			const auto head = _head.load(std::memory_order_acquire);

			return tail == head;
		}
};