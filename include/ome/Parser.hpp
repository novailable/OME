#pragma once

#include <unordered_map>
#include <string>
#include <initializer_list>
#include <cstdint>
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

inline thread_local ErrCode parser_errno = ErrCode::None;

class   Parser
{
    private:
        static constexpr    char SOH = '\x01';
        size_t _body_len = 0;
        bool    _valid = false;
        std::unordered_map<int, std::string_view>    _fields;

        bool    extract(std::string_view item, int &tag, std::string_view &value);
        bool    checksum(std::string_view raw, size_t start);
        bool    tagcheck(std::initializer_list<int> tags);
        bool    bodytag();
        bool    fieldformats();

        template <typename T>
        bool    num_format(T &num, std::string_view n, size_t size = std::string_view::npos)
        {
            size = (size == std::string_view::npos) ? n.size() : size;
            std::from_chars_result result = std::from_chars(n.data(), n.data() + size, num);
            return (result.ec == std::errc{} && result.ptr == n.data() + size);
        }

    public:
        Parser(std::string_view raw);
        [[nodiscard]]
        bool    get_header(std::string_view raw, size_t &start);
        bool    valid() const;
        void    view_fileds() const;
};
