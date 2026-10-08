enum struct Side : uint8_t {
    Buy,
    Sell
};

enum struct Type : uint8_t {
    Limit,
    Market
    // Add Stop, IOC, FOK etc later if you want — don't over-engineer day one
};

enum struct Status : uint8_t {
    New,
    PartiallyFilled,
    Filled,
    Cancelled,
    Rejected
};

using OrderId    = uint64_t;
using ClientId   = uint64_t;
using Price      = int64_t;
using Quantity   = uint64_t;
using Symbol     = std::string;

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
        Quantity    _filled_qty = 0;
        Status      _status = OrderStatus::New;
        uint64_t    _timestamp;

    public:
        Order(OrderId id, ClientId client_id, Symbol symbol, Side side,
            Type type, Price price, Quantity quantity, uint64_t timestamp);

        void fill(Quantity qty); // asserts qty <= remaining(), updates status
        void cancel();

        Quantity remaining() const { return quantity_ - filled_qty_; }
        Status status() const { return status_; }
        // getters for id, symbol, side, price, etc.
};
