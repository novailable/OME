#pragma once

#include "Order.hpp"

struct Trade
{
    OrderId  buy_id;
    OrderId  sell_id;

    Symbol   symbol;

    Price    price;
    Quantity quantity;

    uint64_t timestamp;
};
