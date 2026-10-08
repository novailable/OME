#include "Ome.hpp"
#include "Parser.hpp"

void run_all_tests();
std::string sanitize_fix(std::string msg);

Ome::Ome() : _server() {}

Ome::~Ome() {}

void    Ome::run()
{
    _server.run();
}

void    Ome::test_parser()
{
    std::string msg = "8=FIX.4.4|9=72|35=1|49=BuySide|56=SellSide|34=2|52=20190605-16:56:17.419|112=TestReqID|10=215|8=FIX.4.4|9=72|35=1|49=BuySide|";
    Parser  parser(sanitize_fix(msg));
    // run_all_tests();
    parser.view_fileds();
    std::cout << parser.valid() << std::endl;
}

static SPSCQueue<Order, 1024> g_queue;
static std::atomic<bool> g_producer_done{false};

static void producer()
{
    // A small hand-written sequence of orders designed to produce a
    // few visible trades plus some orders left resting in the book.
    std::vector<Order> orders = {
        {1, 100, "BTCUSD", Side::Sell, Type::Limit, 101, 50, 1}, // rests: bid 100 x10
        {2, 102, "BTCUSD", Side::Sell, Type::Limit, 102, 5, 0}, // rests: ask 102 x5  (no cross, 100 < 102)
        {3, 102, "BTCUSD", Side::Sell, Type::Limit, 102, 5, 0}, // crosses ask 102 -> trades 5 @102, ask emptied
        {4, 103, "BTCUSD", Side::Sell, Type::Limit, 99, 8, 4}, // crosses bid 100 -> trades 8 @100, bid 100 left qty 2
		{5, 104, "BTCUSD", Side::Sell, Type::Limit, 99, 5, 5}, // crosses remaining bid 100 x2 @100, rests ask 99 x18
       	{6, 105, "BTCUSD", Side::Buy, Type::Limit, 101, 2, 6} // crosses resting ask 99 -> trade 3 @99, ask 99 left 15
    };

    for (auto o : orders)
    {
        // spin until there's room -- queue is tiny here so this is
        // effectively instant, but this is the backpressure point
        // in a real system.
        while (!g_queue.push(o))
            std::this_thread::yield();
 
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    g_producer_done.store(true, std::memory_order_release);
}

static void	matcher()
{
	OrderBook book;
    Order order(0,0, "Placeholder", Side::Buy, Type::Limit, 0, 0, 0);
 
    for (;;)
    {
        if (g_queue.pop(order))
        {
			auto	order_ptr = std::make_shared<Order>(order);
			book.add(order_ptr);
            std::vector<Trade> trades = book.trades();
            for (const auto& t : trades)
            {
                std::cout << "TRADE: buy#" << t.buy_id
                          << " x sell#" << t.sell_id
                          << " qty=" << t.quantity
                          << " px=" << t.price << "\n";
            }
			book.clear_trades();
            continue;
        }
 
        // queue looked empty -- but only stop once we also know the
        // producer is finished AND we've drained whatever it pushed
        // right before setting the flag. acquire here pairs with the
        // producer's release store above.
        if (g_producer_done.load(std::memory_order_acquire))
        {
            if (!g_queue.pop(order)) // final drain check
                break;
			auto	order_ptr = std::make_shared<Order>(order);
			book.add(order_ptr);
            std::vector<Trade> trades = book.trades();
            for (const auto& t : trades)
            {
                std::cout << "TRADE: buy#" << t.buy_id
                          << " x sell#" << t.sell_id
                          << " quantity=" << t.quantity
                          << " px=" << t.price << "\n";
            }
            continue;
        }
 
        std::this_thread::yield();
    }
 
    std::cout << "\nFinal book state:\n";
    book.print_book();
}

void	Ome::test_engine()
{
	std::thread producer_thread(producer);
    std::thread matcher_thread(matcher);
 
    producer_thread.join();
    matcher_thread.join();
}