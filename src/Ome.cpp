#include "Ome.hpp"
#include "Parser.hpp"

Ome::Ome() : _server() {}

Ome::~Ome() {}

void    Ome::run()
{
    _server.run();
}

void    Ome::test_parser(std::string &msg)
{
    Parser  parser(msg);
    parser.view_fileds();
}