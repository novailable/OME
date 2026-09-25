#include "Ome.hpp"
#include "Parser.hpp"

void run_all_tests();
std::string sanitize_fix(std::string msg);

Ome::Ome() : _server() {}

Ome::~Ome() {}

void    Ome::run()
{
    _server.run();
}

void    Ome::test_parser()
{

    
    std::string msg = "8=FIX.4.4|9=72|35=1|49=BuySide|56=SellSide|34=2|52=20190605-16:56:17.419|112=TestReqID|10=215|8=FIX.4.4|9=72|35=1|49=BuySide|";
    Parser  parser(sanitize_fix(msg));
    // run_all_tests();
    parser.view_fileds();
    std::cout << parser.valid() << std::endl;
}