// #include "Parser.hpp"
#include "Server.hpp"
#include "Reactor.hpp"
#include "Ome.hpp"

int	main()
{
    std::string ip = "127.0.0.1", port = "8080";
    Ome ome;
    // ome.run();
    ome.test_parser();

    // std::cout << parser[8] << std::endl;
	std::cout << "Cmake is working" << std::endl;
}
