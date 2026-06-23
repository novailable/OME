#include "Parser.hpp"
#include "Server.hpp"

int	main()
{
    std::string ip = "127.0.0.1", port = "8080";
    std::string msg =
        "8=FIX.4.2\x01" "9=178\x01" "35=8\x01" "49=PHLX\x01" "56=PERS\x01"
        "52=20071123-05:30:00.000\x01" "11=ATOMNOCCC9990900\x01" "20=3\x01"
        "150=E\x01" "39=E\x01" "55=MSFT\x01" "167=CS\x01" "54=1\x01" "38=15\x01"
        "40=2\x01" "44=15\x01" "58=PHLX EQUITY TESTING\x01" "59=0\x01" "47=C\x01"
        "32=0\x01" "31=0\x01" "151=15\x01" "14=0\x01" "6=0\x01" "10=128\x01";	
	Parser parser(msg);
    Server  server;
    server.run(ip, port);
    
    std::cout << parser[8] << std::endl;
	std::cout << "Cmake is working" << std::endl;
}