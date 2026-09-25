#pragma once

#include <vector>
#include <string>
#include <initializer_list>
#include <cstdint>
#include <string_view>
#include <charconv>
#include <iostream>

// FIX.4.2

constexpr uint8_t   T34 = 1 << 0;
constexpr uint8_t   T35 = 1 << 1;
constexpr uint8_t   T49 = 1 << 2;
constexpr uint8_t   T56 = 1 << 3;
constexpr uint8_t   T52 = 1 << 4;

struct  Field
{
    int tag;
    std::string_view    value;
};

enum struct ErrCode : uint8_t
{
    None,
    BadCheckSum,
    BadBodyLength,
    MissingRequiredTag,
    DuplicateTag,
    UnknownMsgType,
    MalformedField,
    InvalidEnum
};

// std::iostream std::operator<<(std::ostream &out, ErrCode parser_errno)
// {
//     std::string output;

//     switch(parser_errno)
//     {
//         case ErrCode::None : "Nth wrong"; break;
//         case ErrCode::BadCheckSum : "Bad Check Sum" ; break;
//         case ErrCode::
//     }
// }

inline thread_local ErrCode parser_errno = ErrCode::None;

class   Parser
{
    private:
        static constexpr    char SOH = '\x01';
        uint8_t _tag_check = 0;
        size_t _body_len = 0;
        bool    _valid = false;
        std::vector<Field>    _fields;

        bool    extract(std::string_view item, int &tag, std::string_view &value);
        bool    checksum(std::string_view raw, size_t start);
        bool    requiredtag(int tag);
        bool    tagcheck(unsigned tags);
        // bool    bodytag();
        // bool    fieldformats();

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
        std::string_view    get(int tag);
        bool    valid() const;
        void    view_fileds() const;
};
