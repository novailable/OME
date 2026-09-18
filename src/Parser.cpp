#include "Parser.hpp"

bool    Parser::extract(std::string_view item, int &tag, std::string_view &value)
{
    size_t  eq_pos = item.find('=');

    if (eq_pos == std::string_view::npos)
        return (false);
    if (!num_format(tag, item, eq_pos) || tag <= 0)
        return (false);
    value = item.substr(eq_pos + 1);
    return (true);
}

bool    Parser::tagcheck(std::initializer_list<int> tags)
{
    for (int tag : tags)
    {
        if (_fields.find(tag) == _fields.end())
        {
            parser_errno = ErrCode::MissingRequiredTag;
            return (false);
        }
    }
    return (true);
}

Parser::Parser(std::string_view raw)
{
    size_t  start = 0, pos;

    if (!get_header(raw, start))
        return;

    if (_body_len <= 0 || start + static_cast<size_t>(_body_len) > raw.size())
    {
        parser_errno = ErrCode::BadBodyLength;
        return;
    }

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

    if (!checksum(raw, start))
        return ;
    if (!tagcheck({35, 34, 49, 56}))
        return ;
    if (!bodytag())
        return ;
    if (!fieldformats())
        return ;
    _valid = true;
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

    if (!num_format(_body_len, value) || _body_len == 0)
        return (false);
    return (true);
}

bool    Parser::fieldformats()
{
    std::unordered_map<int, std::string_view>
    ::iterator  side = _fields.find(54);
    if (side != _fields.end())
    {
        if (side->second.size() != 1 || (side->second[0] != '1' && side->second[0] != '2'))
            return (parser_errno = ErrCode::InvalidEnum, false);
    }

    std::unordered_map<int, std::string_view>
    ::iterator  qty = _fields.find(38);
    if (qty != _fields.end())
    {
        double  qty_value;
        if (!num_format(qty_value, qty->second) || qty_value <= 0)
            return (parser_errno = ErrCode::MalformedField, false);
    }

    std::unordered_map<int, std::string_view>
    ::iterator  price = _fields.find(44);
    if (price != _fields.end())
    {
        double  price_value;
        if (!num_format(price_value, price->second) || price_value <= 0)
            return (parser_errno = ErrCode::MalformedField, false);
    }
    return (true);
}

bool    Parser::checksum(std::string_view raw, size_t start)
{
    size_t  pos = raw.find(SOH, start);
    if (pos == std::string_view::npos)
        return(parser_errno = ErrCode::BadBodyLength, false);

    int tag;
    std::string_view value;
    if (!extract(raw.substr(start, pos - start), tag, value) || tag != 10)
        return(parser_errno = ErrCode::BadBodyLength, false);
    _fields[10] = value;

    int check_sum;
    if (!num_format(check_sum, value))
		return (parser_errno = ErrCode::BadCheckSum, false);

    unsigned int sum = 0;
    for (unsigned char c : raw.substr(0, start))
        sum += c;
    if (static_cast<uint8_t>(sum % 256) != check_sum)
        return (parser_errno = ErrCode::BadCheckSum, false);
    return (true);
}

bool    Parser::bodytag()
{
    std::unordered_map<int, std::string_view>
    ::iterator  typeIt = _fields.find(35);

    if (typeIt == _fields.end())
        return (parser_errno = ErrCode::MissingRequiredTag, false);

    const std::string_view  &type = typeIt->second;
    if (type == "D")
    {
        if (!tagcheck({11, 55, 54, 38, 40}))
            return (false);
        if (_fields[40] == "2" && _fields.find(44) == _fields.end())
            return (parser_errno = ErrCode::MissingRequiredTag, false);
    }
    return (true);
}

void    Parser::view_fileds() const
{
    for (const auto& [key, value] : _fields)
    {
        std::cout << key << "\t|\t" << value << std::endl;
    }
}

bool    Parser::valid() const
{
    return (_valid);
}
