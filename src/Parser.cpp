#include "Parser.hpp"


bool    extract(std::string_view item, int &tag, std::string_view &value)
{
    size_t  eq_pos = item.find('=');
    if (eq_pos != std::string_view::npos)
    {
        std::from_chars_result result = std::from_chars(item.data(), item.data() + eq_pos, tag);
        if (result.ec != std::errc{} || result.ptr != item.data() + eq_pos)
            return (false);
        value = item.substr(eq_pos + 1);
        return (true);
    }
    return (false);
}

bool    Parser::get_header(std::string_view raw, size_t &start)
{
    size_t pos;
    int tag;
    std::string_view    value;

    if ((pos = raw.find(SOH, start)) == std::string_view::npos)
        return (false);
    if ((!extract(raw.substr(start, pos - start), tag, value) || tag != 8))
        return (false);
    _fields[8] = value;
    start = pos + 1;

    if ((pos = raw.find(SOH, start)) == std::string_view::npos)
        return (false);
    if (!extract(raw.substr(start, pos - start), tag, value) || tag != 9)
        return (false);
    _fields[9] = value;
    start = pos + 1;

    std::from_chars_result result = std::from_chars(value.data(), value.data() + value.size(), _body_len);
    if (result.ec != std::errc{} || _body_len < 0)
        return (false);
    return (true);
}

Parser::Parser(std::string_view raw)
{
    size_t  start = 0, pos;

    if (!get_header(raw, start))
        return;

    if (start + _body_len > raw.size())
        return; // declared body longer than what we actually have

    std::string_view body = raw.substr(start, _body_len);
    size_t bstart = 0, bpos;

    while ((bpos = body.find(SOH, bstart)) != std::string_view::npos)
    {
        int tag;
        std::string_view item(body.data() + bstart, bpos - bstart), value;
        if (extract(item, tag, value))
            _fields[tag] = value;
        bstart = bpos + 1;
    }

    start += _body_len;
    if ((pos = raw.find(SOH, start)) == std::string_view::npos)
        return;

    int tag;
    std::string_view value;
    if (!extract(raw.substr(start, pos - start), tag, value) || tag != 10)
        return;

    _fields[10] = value;
    _valid = true;
}

void    Parser::view_fileds() const
{
    for (const auto& [key, value] : _fields)
    {
        std::cout << key << "|" << value << std::endl;
    }
}

// namespace{
//     bool    check_head()
//     {
//         if (tag_ )
//     }
// }

// bool    valid

bool    Parser::validation(int tag, std::string_view value)
{
    if (_fields.size() == 0 && tag != 8)
        return (false);
    return (true);
}

bool    Parser::valid() const
{
    return (_valid);
}