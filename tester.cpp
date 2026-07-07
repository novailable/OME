#include "Parser.hpp"
#include "Server.hpp"
#include <gtest/gtest.h>

TEST(ParserTest, MultipleTags)
{
    Parser p("35=HELLO|49=CLIENT|56=SERVER");

    EXPECT_EQ(p.get(35).value(), "HELLO");
    EXPECT_EQ(p.get(49).value(), "CLIENT");
    EXPECT_EQ(p.get(56).value(), "SERVER");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}