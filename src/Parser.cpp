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

std::string_view    Parser::get(int tag)
{
    for (const Field& field : _fields)
    {
        if (field.tag == tag)
            return (field.value);
    }
    return {};
}

bool    Parser::tagcheck(unsigned tags)
{
    _tag_check = tags & ~_tag_check;
    if (_tag_check)
    {
    //     int tag = tags[std::count_zero(_tag_check)];
        parser_errno = ErrCode::MissingRequiredTag;
        return (false);
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
        if (!extract(item, tag, value))
        {
            parser_errno = ErrCode::MalformedField;
            return ;
        }
        if (!requiredtag(tag))
            return ;
        _fields.push_back({tag, value});
        bstart = bpos + 1;
    }

    start += _body_len;
    if (!checksum(raw, start))
        return ;
    if (!tagcheck(T34 | T35 | T49 | T52 | T56))
        return ;
    // if (!bodytag())
    //     return ;
    // if (!fieldformats())
    //     return ;
    _valid = true;
}

bool    Parser::requiredtag(int tag)
{
    uint8_t bit = 0;

    switch (tag)
    {
        case 34: bit = T34; break;
        case 35: bit = T35; break;
        case 49: bit = T49; break;
        case 56: bit = T56; break;
        case 52: bit = T52; break;
        default:
            return (true);
    }

    if (_tag_check & bit)
    {
        parser_errno = ErrCode::DuplicateTag;
        return (false);
    }

    _tag_check |= bit;
    return (true);
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
    // _fields.push_back({8, value});
    start = pos + 1;

    if ((pos = raw.find(SOH, start)) == std::string_view::npos)
        return (false);
    if (!extract(raw.substr(start, pos - start), tag, value) || tag != 9)
        return (false);
    // _fields.push_back({9, value});
    start = pos + 1;

    if (!num_format(_body_len, value) || _body_len == 0)
        return (false);
    return (true);
}

// bool    Parser::fieldformats()
// {
//     std::unordered_map<int, std::string_view>
//     ::iterator  side = _fields.find(54);
//     if (side != _fields.end())
//     {
//         if (side->second.size() != 1 || (side->second[0] != '1' && side->second[0] != '2'))
//             return (parser_errno = ErrCode::InvalidEnum, false);
//     }

//     std::unordered_map<int, std::string_view>
//     ::iterator  qty = _fields.find(38);
//     if (qty != _fields.end())
//     {
//         double  qty_value;
//         if (!num_format(qty_value, qty->second) || qty_value <= 0)
//             return (parser_errno = ErrCode::MalformedField, false);
//     }

//     std::unordered_map<int, std::string_view>
//     ::iterator  price = _fields.find(44);
//     if (price != _fields.end())
//     {
//         double  price_value;
//         if (!num_format(price_value, price->second) || price_value <= 0)
//             return (parser_errno = ErrCode::MalformedField, false);
//     }
//     return (true);
// }

bool    Parser::checksum(std::string_view raw, size_t start)
{
    size_t  pos = raw.find(SOH, start);
    if (pos == std::string_view::npos)
        return(parser_errno = ErrCode::BadBodyLength, false);

    int tag;
    std::string_view value;
    if (!extract(raw.substr(start, pos - start), tag, value) || tag != 10)
        return(parser_errno = ErrCode::BadBodyLength, false);

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

// bool    Parser::bodytag()
// {
//     std::unordered_map<int, std::string_view>
//     ::iterator  typeIt = _fields.find(35);

//     if (typeIt == _fields.end())
//         return (parser_errno = ErrCode::MissingRequiredTag, false);

//     const std::string_view  &type = typeIt->second;
//     if (type == "D")
//     {
//         if (!tagcheck({11, 55, 54, 38, 40}))
//             return (false);
//         if (_fields[40] == "2" && _fields.find(44) == _fields.end())
//             return (parser_errno = ErrCode::MissingRequiredTag, false);
//     }
//     return (true);
// }

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
