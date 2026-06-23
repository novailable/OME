#pragma once

#include <string>
#include <string_view>
#include <charconv>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <optional>

#define DELIM '\x01'
#define EQUAL '='

// "8=FIX.4.2\x019=178\x0135=8\x0149=PHLX\x0156=PERS\x0152=20071123-05:30:00.000\x0111=ATOMNOCCC9990900\x0120=3\x01150=E\x0139=E\x0155=MSFT\x01167=CS\x0154=1\x0138=15\x0140=2\x0144=15\x0158=PHLX EQUITY TESTING\x0159=0\x0147=C\x0132=0\x0131=0\x01151=15\x0114=0\x016=0\x0110=128\x01";

std::vector<std::string>	split_msg(const std::string &msg);
void	print_container(const std::vector<std::string> &container);

class Parser
{
	private:
		std::string	_raw;
		std::map<int, std::string_view>	_items;

	public:
		Parser(std::string msg);
		void	print();
		std::optional<std::string_view>	get(int tag) const;

		std::optional<std::string_view>	operator[](int tag) const;
};

std::ostream	&operator<<(std::ostream& out, const std::optional<std::string_view>& data);
