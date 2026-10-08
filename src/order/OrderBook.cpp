#include "OrderBook.hpp"
#include <algorithm>
#include <iostream>

void OrderBook::add(OrderPtr order)
{
    if (!order)
        return;
    if (order->quantity() == 0)
        return;

    if (order->side() == Side::Buy)
        match_buy(order);
    else
        match_sell(order);

    // Fully filled against the resting book -- never rests, so it
    // never enters _orders/_locations at all.
    if (order->remaining() == 0)
        return;

    // A market order cannot rest in the book. Whatever could be
    // matched already was, above; the remainder is dead.
    if (order->type() == Type::Market)
    {
        order->cancel();
        return;
    }

    // Rest it, and remember exactly where it landed.
    if (order->side() == Side::Buy)
    {
        auto& lvl = _bids[order->price()].orders;
        lvl.push_back(order);
        _locations[order->id()] = { order->price(), std::prev(lvl.end()) };
    }
    else
    {
        auto& lvl = _asks[order->price()].orders;
        lvl.push_back(order);
        _locations[order->id()] = { order->price(), std::prev(lvl.end()) };
    }

    _orders[order->id()] = order;
}

void OrderBook::match_buy(OrderPtr incoming)
{
    while (incoming->remaining() > 0 && !_asks.empty())
    {
        auto level_it = _asks.begin();
        Price ask_price = level_it->first;

        if (incoming->type() == Type::Limit && ask_price > incoming->price())
            break;

        PriceLevel& level = level_it->second;

        while (!level.orders.empty() && incoming->remaining() > 0)
        {
            OrderPtr resting = level.orders.front();
            Quantity quantity = std::min(incoming->remaining(), resting->remaining());

            execute(incoming, resting, quantity, resting->price());

            if (resting->remaining() == 0)
            {
                _orders.erase(resting->id());
                _locations.erase(resting->id());
                level.orders.pop_front();
            }
        }

        if (level.orders.empty())
            _asks.erase(level_it);
    }
}

void OrderBook::match_sell(OrderPtr incoming)
{
    while (incoming->remaining() > 0 && !_bids.empty())
    {
        auto level_it = _bids.begin();
        Price bid_price = level_it->first;

        if (incoming->type() == Type::Limit && bid_price < incoming->price())
            break;

        PriceLevel& level = level_it->second;

        while (!level.orders.empty() && incoming->remaining() > 0)
        {
            OrderPtr resting = level.orders.front();
            Quantity quantity = std::min(incoming->remaining(), resting->remaining());

            execute(incoming, resting, quantity, resting->price());

            if (resting->remaining() == 0)
            {
                _orders.erase(resting->id());
                _locations.erase(resting->id());
                level.orders.pop_front();
            }
        }

        if (level.orders.empty())
            _bids.erase(level_it);
    }
}

void OrderBook::execute(const OrderPtr& incoming, const OrderPtr& resting,
                         Quantity quantity, Price price)
{
    if (quantity == 0)
        return;

    incoming->fill(quantity);
    resting->fill(quantity);

    Trade trade{
        .buy_id = incoming->side() == Side::Buy ? incoming->id() : resting->id(),
        .sell_id = incoming->side() == Side::Sell ? incoming->id() : resting->id(),
        .symbol = incoming->symbol(),
        .price = price,
        .quantity = quantity,
        .timestamp = ++_timestamp
    };

    _trades.push_back(trade);

    std::cout << "[TRADE] " << trade.quantity << " @ " << trade.price
              << " | buy=" << trade.buy_id << " sell=" << trade.sell_id << '\n';
}

bool OrderBook::cancel(OrderId id)
{
    auto order_it = _orders.find(id);

    // Not currently resting (never rested, already filled, or
    // already cancelled) -- nothing to do.
    if (order_it == _orders.end())
        return false;

    OrderPtr order = order_it->second;

    if (order->status() == Status::Filled)
        return false;

    auto loc_it = _locations.find(id);
    if (loc_it == _locations.end())
        return false; // defensive: an entry in _orders should always have a location

    const BookLocation loc = loc_it->second;

    // No scan: we already know exactly which node to erase, because
    // we saved the iterator when the order was first inserted.
    auto erase_from_book = [&](auto& book)
    {
        auto level_it = book.find(loc.price);
        if (level_it == book.end())
            return;

        level_it->second.orders.erase(loc.it); // O(1)

        if (level_it->second.orders.empty())
            book.erase(level_it);
    };

    if (order->side() == Side::Buy)
        erase_from_book(_bids);
    else
        erase_from_book(_asks);

    order->cancel();
    _orders.erase(order_it);
    _locations.erase(loc_it);

    return true;
}

void OrderBook::print_book() const
{
    std::cout << "\n========== ORDER BOOK ==========\n";
    std::cout << "ASKS\n";
    for (const auto& [price, level] : _asks)
    {
        Quantity total = 0;
        for (const auto& order : level.orders)
            total += order->remaining();
        std::cout << price << " x " << total << '\n';
    }
    std::cout << "-------------------------------\n";
    std::cout << "BIDS\n";
    for (const auto& [price, level] : _bids)
    {
        Quantity total = 0;
        for (const auto& order : level.orders)
            total += order->remaining();
        std::cout << price << " x " << total << '\n';
    }
    std::cout << "================================\n";
}