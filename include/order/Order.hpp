#pragma once

#include <cstdint>
#include <string>

enum struct Side : uint8_t
{
    Buy,
    Sell
};

enum struct Type : uint8_t
{
    Limit,
    Market
};

enum struct Status : uint8_t
{
    New,
    PartiallyFilled,
    Filled,
    Cancelled,
    Rejected
};

using OrderId  = uint64_t;
using ClientId = uint64_t;
using Price    = int64_t;
using Quantity = uint64_t;
using Symbol   = std::string;

class Order
{
	private:
		OrderId     _id;
		ClientId    _client_id;
		Symbol      _symbol;
		Side        _side;
		Type        _type;
		Price       _price;
		Quantity    _quantity;
		Quantity    _filled_qty;
		Status      _status;
		uint64_t    _timestamp;

	public:
		Order(
			OrderId id,
			ClientId client_id,
			Symbol symbol,
			Side side,
			Type type,
			Price price,
			Quantity quantity,
			uint64_t timestamp
		);

		OrderId id() const { return _id; }
		ClientId client_id() const { return _client_id; }

		const Symbol& symbol() const { return _symbol; }

		Side side() const { return _side; }
		Type type() const { return _type; }

		Price price() const { return _price; }
		Quantity quantity() const { return _quantity; }
		Quantity filled_qty() const { return _filled_qty; }

		Quantity remaining() const
		{
			return _quantity - _filled_qty;
		}

		Status status() const
		{
			return _status;
		}

		uint64_t timestamp() const
		{
			return _timestamp;
		}

		void fill(Quantity qty);
		void cancel();
};

