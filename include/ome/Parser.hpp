#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
#include <charconv>
#include <iostream>

class   Parser
{
    private:
        static constexpr    char SOH = '\x01';
        int _body_len = 0;
        bool    _valid = false;
        std::unordered_map<int, std::string>    _fields;
    public:
        Parser(std::string_view raw);
        [[nodiscard]]
        bool    has(int tag) const;

        bool    get_header(std::string_view raw, size_t &start);
        bool    valid() const;
        bool    validation(int tag, std::string_view value);
        void    view_fileds() const;
};
