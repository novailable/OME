#pragma once
#include <vector>
#include <string_view>
#include <optional>
#include <cstdint>
// #include "NewOrderRequest.h"

class Parser {
public:
    void feed(const char* data, size_t len);

    // Parses as many complete messages as are available, moving each
    // directly into the returned vector. No intermediate map, no per-byte copy.
    std::vector<NewOrderRequest> extractOrders();

    void reset();

private:
    std::vector<char> buffer_;
    size_t read_pos_ = 0; // where unconsumed data starts

    static constexpr char SOH = '\x01';

    // View-only scan — no copying, no allocation, just pointers into buffer_.
    std::optional<size_t> tryFindCompleteMessage(std::string_view view) const;

    NewOrderRequest parseOrder(std::string_view msg) const;

    void compactIfNeeded();
};