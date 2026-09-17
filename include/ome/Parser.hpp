#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
#include <charconv>
#include <iostream>

enum struct ErrCode : uint8_t
{
    None,
    BadCheckSum,
    BadBodyLength,
    MissingRequiredTag,
    UnknownMsgType,
    MalformedField,
    InvalidEnum
};

inline thread_local ErrCode parser_errno = 0;

class   Parser
{
    private:
        static constexpr    char SOH = '\x01';
        size_t _body_len = 0;
        bool    _valid = false;
        std::unordered_map<int, std::string_view>    _fields;
        bool    checksum(std::string_view raw, size_t start);
        bool    tagcheck(std::initializer_list<int> tags);
        bool    bodytag();
        bool    fieldformats();

        template <typename T>
        bool    num_format(std::string_view n, T &num)
        {
            std::from_chars_result result = std::from_chars(n.data(), n.data() + n.size(), num)
            return (result.ec == std::errc{} && result.ptr == n.data() + n.size());
        }

    public:
        Parser(std::string_view raw);
        [[nodiscard]]
        bool    has(int tag) const;

        bool    get_header(std::string_view raw, size_t &start);
        bool    valid() const;
        // bool    validation(int tag, std::string_view value);
        void    view_fileds() const;
};
