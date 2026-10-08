#pragma once

#include <map>
#include <list>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Order.hpp"
#include "Trade.h"

struct PriceLevel
{
    std::list<std::shared_ptr<Order>> orders;
};

class OrderBook
{
	private:
		using OrderPtr = std::shared_ptr<Order>;

		// Where a resting order's node lives, captured at insertion time,
		// so cancel() never has to scan a price level to find it.
		struct BookLocation
		{
			Price price;
			std::list<OrderPtr>::iterator it;
		};

		std::map<Price, PriceLevel, std::greater<Price>> _bids;
		std::map<Price, PriceLevel, std::less<Price>> _asks;
		std::unordered_map<OrderId, OrderPtr> _orders;        // id -> order object
		std::unordered_map<OrderId, BookLocation> _locations;  // id -> where it rests
		std::vector<Trade> _trades;
		uint64_t _timestamp = 0;

	private:
		void match_buy(OrderPtr incoming);
		void match_sell(OrderPtr incoming);
		void execute(const OrderPtr& incoming, const OrderPtr& resting, Quantity quantity, Price price);

	public:
		void add(OrderPtr order);
		bool cancel(OrderId id);
		const std::vector<Trade>& trades() const { return _trades; }
		void clear_trades() { _trades.clear(); }
		void print_book() const;
};