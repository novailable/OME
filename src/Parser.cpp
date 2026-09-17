#include "Parser.hpp"

static bool    extract(std::string_view item, int &tag, std::string_view &value)
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

    if (!num_format(value, _body_len) || _body_len == 0)
        return (false);
    
    // std::from_chars_result result = std::from_chars(value.data(), value.data() + value.size(), _body_len);
    // if (result.ec != std::errc{} || _body_len < 0)
    //     return (false);
    return (true);
}

Parser::Parser(std::string_view raw)
{
    size_t  start = 0, pos;

    if (!get_header(raw, start))
        return;

    if (start + static_cast<size_t>_body_len > raw.size())
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

    if (!checksum(start))
        return ;
    if (!tagcheck({35, 34, 49, 56}))
        return ;
    if (!bodytag())
        return ;
    if (!fieldformats())
        return ;
}

bool    Parser::fieldformats()
{
    std::unordered_map<int, std::string_view>
    ::iterator  side = _fields.find(54);
    if (side != _fields.end())
    {
        if (side->second.size() != 1 || (size->second[0] != '1' && side->second[0] != '2'))
        {
            parser_errno = ErrCode::InvalidEnum;
            return (false);
        }
    }

    std::unordered_map<int, std::string_view>
    ::iterator  qty = _fields.find(38);
    if (qty != _fields.end())
    {
        double  qty_value;
        if (num_format(qty->second, value) || qty <= 0)
        {
            parser_errno = ErrCode::MalformedField;
            return (false);
        }
    }

    std::unordered_map<int, std::string_view>
    ::iterator  price = _fields.find(44);
    if (price != _fields.end())
    {
        double  price_value;
        if (num_format(price->second, price_value) || price <= 0)
            return (false);
    }
    return (true);
}

bool static Parser::checksum(std::string_view raw, size_t start)
{
    size_t  pos = raw.find(SOH, start);
    if (pos == std::string_view::npos)
        return(parser_errno = ErrCode::BadBodyLength, false);
    
    int tag;
    std::string_view value;
    if (!extract(raw.substr(start, pos - start), tag, value) || tag != 10)
        return(parser_errno = ErrCode::BadBodyLength, false);
    _fields[10] = value;

    int checksum;
	auto r = std::from_chars(value.data(), value.data() + value.size(), checksum);
	if (r.ec != std::errc{} || r.ptr != value.data() + value.size())
	{
		_errno = ErrCode::BadChecksum;
		return (false);
	}

    unsigned_int sum = 0;
    for (unsigned char c : raw)
        sum += c;
    return (static_cast<uint8_t>(sum % 256));
}

bool    Parser::bodytag()
{
    std::unordered_map<int, std::stringview>
    ::iterator  type = _fields.find(35);

    if (type == _fields.end())
    {
        parser_errno = ErrCode::MissingRequiredTag;
        return (false);
    }
    const std::string_view  &type = type->second;
    if (type == "D")
    {
        if (!tagcheck({11, 55, 54, 38, 40}))
            return (false);
        if (_fields[40] == "2" && _fields.find(44) == _fields.end())
        {
            parser_errno = ErrCode::MissingRequiredTag;
            return (false);
        }
        return (true);
    }

    parser_errno = ErrCode::UnknownMsgType;
    return (false);
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

// bool    Parser::validation(int tag, std::string_view value)
// {
//     if (_fields.size() == 0 && tag != 8)
//         return (false);
//     return (true);
// }

bool    Parser::valid() const
{
    return (_valid);
}