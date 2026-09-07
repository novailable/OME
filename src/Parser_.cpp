// #include "Parser.hpp"

// Parser::Parser(std::string msg) : _raw(std::move(msg))
// {
// 	size_t	start = 0, pos;

// 	while ((pos = _raw.find(DELIM, start)) != std::string_view::npos)
// 	{
// 		if (pos > start)
// 		{
// 			std::string_view	item(_raw.data() + start, pos - start);
// 			size_t	eq = item.find(EQUAL);
// 			if (eq != std::string_view::npos)
// 			{
// 				int tag; 
// 				std::from_chars(item.data(), item.data() + eq, tag); 
// 				_items[tag] = item.substr(eq + 1);
// 			}
// 		}
// 		start = pos + 1;
// 	}
// }

// std::optional<std::string_view>	Parser::get(int tag) const
// {
// 	std::map<int, std::string_view>::const_iterator	it = _items.find(tag);
// 	if (it == _items.end())
// 		return std::nullopt;
// 	return it->second;
// }

// std::optional<std::string_view>	Parser::operator[](int tag) const
// {
// 	return get(tag);
// }

// void	Parser::print()
// {
// 	if (_items.empty())
// 	{
// 		std::cout << "nothing inside the parser" << std::endl;
// 		return ;
// 	}
// 	for (std::map<int, std::string_view>::const_iterator it = _items.begin(); it != _items.end(); ++it)
// 		std::cout << "tag: " << it->first << ", value: " << it->second << std::endl;
// }

// std::ostream	&operator<<(std::ostream& out, const std::optional<std::string_view>& data)
// {
// 	if (data.has_value())
// 		out << *data;
// 	else
// 		out << "(null)";
// 	return (out);
// }

#include "Parser.hpp"
#include <charconv>

void Parser::feed(const char* data, size_t len) {
    buffer_.insert(buffer_.end(), data, data + len);
}

void Parser::reset() {
    buffer_.clear();
    read_pos_ = 0;
}

// Instead of erasing from the front every time, just advance read_pos_.
// Only physically shift memory when the "used up" prefix gets large.
void Parser::compactIfNeeded() {
    constexpr size_t COMPACT_THRESHOLD = 4096;
    if (read_pos_ > COMPACT_THRESHOLD) {
        buffer_.erase(buffer_.begin(), buffer_.begin() + read_pos_);
        read_pos_ = 0;
    }
}

std::optional<size_t> Parser::tryFindCompleteMessage(std::string_view view) const {
    size_t firstSoh = view.find(SOH);
    if (firstSoh == std::string_view::npos) return std::nullopt;

    size_t secondSoh = view.find(SOH, firstSoh + 1);
    if (secondSoh == std::string_view::npos) return std::nullopt;

    std::string_view tag9Field = view.substr(firstSoh + 1, secondSoh - firstSoh - 1);
    size_t eq = tag9Field.find('=');
    if (eq == std::string_view::npos || tag9Field.substr(0, eq) != "9") {
        return std::nullopt; // malformed — caller decides how to handle
    }

    int bodyLength = 0;
    auto valueView = tag9Field.substr(eq + 1);
    std::from_chars(valueView.data(), valueView.data() + valueView.size(), bodyLength);

    size_t headerLen = secondSoh + 1;
    size_t checksumFieldLen = 7; // "10=" + 3 digits + SOH
    size_t totalNeeded = headerLen + bodyLength + checksumFieldLen;

    if (view.size() < totalNeeded) return std::nullopt; // incomplete, wait for more bytes

    return totalNeeded;
}

std::vector<NewOrderRequest> Parser::extractOrders() {
    std::vector<NewOrderRequest> orders;

    while (true) {
        std::string_view view(buffer_.data() + read_pos_, buffer_.size() - read_pos_);
        auto msgLen = tryFindCompleteMessage(view);
        if (!msgLen) break; // partial message — leave it, exactly like before

        std::string_view completeMsg = view.substr(0, *msgLen);
        orders.push_back(parseOrder(completeMsg)); // the ONE real copy: fields into the struct

        read_pos_ += *msgLen;
    }

    compactIfNeeded();
    return orders;
}

NewOrderRequest Parser::parseOrder(std::string_view msg) const {
    NewOrderRequest req{};
    size_t pos = 0;

    while (pos < msg.size()) {
        size_t soh = msg.find(SOH, pos);
        if (soh == std::string_view::npos) break;

        std::string_view field = msg.substr(pos, soh - pos);
        size_t eq = field.find('=');
        if (eq != std::string_view::npos) {
            std::string_view tagStr = field.substr(0, eq);
            std::string_view value  = field.substr(eq + 1);

            int tag = 0;
            std::from_chars(tagStr.data(), tagStr.data() + tagStr.size(), tag);

            switch (tag) {
                case 11: req.cl_ord_id = std::string(value); break; // small string, copy is fine
                case 55: req.symbol    = std::string(value); break;
                case 54: req.side      = (value == "1") ? Side::Buy : Side::Sell; break;
                case 38: std::from_chars(value.data(), value.data() + value.size(), req.quantity); break;
                case 44: /* parse price as fixed-point, see note below */ break;
                case 40: req.type = (value == "1") ? OrderType::Market : OrderType::Limit; break;
                default: break; // ignore tags you don't care about yet
            }
        }
        pos = soh + 1;
    }
    return req; // NRVO / move, no copy on return
}