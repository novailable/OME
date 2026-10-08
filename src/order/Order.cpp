#include "Order.hpp"
#include <stdexcept>

Order::Order(
    OrderId id,
    ClientId client_id,
    Symbol symbol,
    Side side,
    Type type,
    Price price,
    Quantity quantity,
    uint64_t timestamp
)
    : _id(id),
      _client_id(client_id),
      _symbol(std::move(symbol)),
      _side(side),
      _type(type),
      _price(price),
      _quantity(quantity),
      _filled_qty(0),
      _status(Status::New),
      _timestamp(timestamp)
{
}

void Order::fill(Quantity qty)
{
    if (qty == 0 || qty > remaining())
        throw std::logic_error("invalid fill quantity");

    _filled_qty += qty;

    if (_filled_qty == _quantity)
        _status = Status::Filled;
    else
        _status = Status::PartiallyFilled;
}

void Order::cancel()
{
    if (_status == Status::Filled)
        return;

    _status = Status::Cancelled;
}

