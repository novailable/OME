#include "Ome.hpp"

Ome::Ome() : _server() {}

Ome::~Ome() {}

void    Ome::run()
{
    _server.run();
}