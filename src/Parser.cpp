#include "Parser.hpp"

Parser::Parser(std::string msg) : _raw(std::move(msg))
{
	size_t	start = 0, pos;

	while ((pos = _raw.find(DELIM, start)) != std::string_view::npos)
	{
		if (pos > start)
		{
			std::string_view	item(_raw.data() + start, pos - start);
			size_t	eq = item.find(EQUAL);
			if (eq != std::string_view::npos)
			{
				int tag;
				std::from_chars(item.data(), item.data() + eq, tag); 
				_items[tag] = item.substr(eq + 1);
			}
		}
		start = pos + 1;
	}
}

std::optional<std::string_view>	Parser::get(int tag) const
{
	std::map<int, std::string_view>::const_iterator	it = _items.find(tag);
	if (it == _items.end())
		return std::nullopt;
	return it->second;
}

std::optional<std::string_view>	Parser::operator[](int tag) const
{
	return get(tag);
}

void	Parser::print()
{
	if (_items.empty())
	{
		std::cout << "nothing inside the parser" << std::endl;
		return ;
	}
	for (std::map<int, std::string_view>::const_iterator it = _items.begin(); it != _items.end(); ++it)
		std::cout << "tag: " << it->first << ", value: " << it->second << std::endl;
}

std::ostream	&operator<<(std::ostream& out, const std::optional<std::string_view>& data)
{
	if (data.has_value())
		out << *data;
	else
		out << "(null)";
	return (out);
}