#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Parser.hpp"

// Utility to replace visual pipe symbols '|' with actual FIX SOH (\x01) delimiters
inline std::string sanitize_fix(std::string msg) {
    std::replace(msg.begin(), msg.end(), '|', '\x01');
    return msg;
}

// Struct to hold individual test case definitions
struct TestCase {
    int id;
    std::string name;
    std::string raw_msg;
    bool expected_valid;
};

// Runner function: compares actual parser output with expected boolean
bool run_test(const TestCase& test) {
    std::string formatted_msg = sanitize_fix(test.raw_msg);
    bool actual_result = false;

    try {
        Parser parse(formatted_msg);
        // Assuming parse.isValid() or parse.parse() returns a boolean outcome.
        // Adjust this method call to match your Parser class API:
        actual_result = parse.valid(); 
    } 
    catch (...) {
        // If your parser throws on invalid input instead of returning false
        actual_result = false; 
    }

    bool passed = (actual_result == test.expected_valid);

    if (!passed)
        std::cout << "parser err: " << (int)parser_errno << std::endl;
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] Test " 
              << (test.id < 10 ? "0" : "") << test.id << ": " 
              << test.name << std::endl;

    if (!passed) {
        std::cout << "   Expected: " << (test.expected_valid ? "true" : "false")
                  << " | Got: " << (actual_result ? "true" : "false") << "\n";
    }

    return passed;
}

// Execution suite for all 44 FIX message scenarios
void run_all_tests() {
    const std::vector<TestCase> tests = {
        // ID, Description, Raw FIX Message, Expected Result
        {1, "Valid New Order Single", "8=FIX.4.4|9=72|35=1|49=BuySide|56=SellSide|34=2|52=20190605-16:56:17.419|112=TestReqID|10=215|", true},
        {2, "Empty message", "", false},
        {3, "Missing SOH", "8=FIX.4.49=10535=D34=149=CLIENT56=SERVER11=ORDER155=AAPL54=138=10040=244=150.5010=118", false},
        {4, "Wrong first tag", "35=D|8=FIX.4.4|9=105|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {5, "Missing 9", "8=FIX.4.4|35=D|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {6, "Invalid BodyLength", "8=FIX.4.4|9=ABC|35=D|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {7, "BodyLength = 0", "8=FIX.4.4|9=0|35=D|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {8, "BodyLength too small", "8=FIX.4.4|9=5|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|38=100|40=2|44=150.50|10=118|", false},
        {9, "BodyLength too large", "8=FIX.4.4|9=500|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|38=100|40=2|44=150.50|10=118|", false},
        {10, "Body doesn't end at SOH", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|38=100|40=2|44=150.5010=118|", false},
        {11, "Malformed field", "8=FIX.4.4|9=105|35D|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {12, "Non-numeric tag", "8=FIX.4.4|9=105|XX=D|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {13, "Empty tag", "8=FIX.4.4|9=105|=D|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {14, "Empty field", "8=FIX.4.4|9=105|35=|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {15, "Duplicate tag", "8=FIX.4.4|9=105|35=D|34=1|34=2|49=CLIENT|56=SERVER|10=118|", false},
        {16, "Missing checksum", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|", false},
        {17, "Wrong checksum", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|38=100|40=2|44=150.50|10=999|", false},
        {18, "Non-numeric checksum", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|10=ABC|", false},
        {19, "2-digit checksum", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|10=18|", false},
        {20, "4-digit checksum", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|10=0118|", false},
        {21, "Checksum > 255", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|10=256|", false},
        {22, "Data after checksum", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|10=118|EXTRA", false},
        {23, "Missing 35", "8=FIX.4.4|9=105|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {24, "Missing 34", "8=FIX.4.4|9=105|35=D|49=CLIENT|56=SERVER|10=118|", false},
        {25, "Missing 49", "8=FIX.4.4|9=105|35=D|34=1|56=SERVER|10=118|", false},
        {26, "Missing 56", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|10=118|", false},
        {27, "35=D valid", "8=FIX.4.4|9=100|35=D|34=1|49=CLIENT|56=SERVER|52=20260925-16:46:00.000|11=ORDER1|55=AAPL|54=1|38=100|40=2|44=150.50|10=106|", true},        {28, "35=D missing 11", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|55=AAPL|54=1|38=100|40=2|44=150.50|10=118|", false},
        {29, "35=D missing 55", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|54=1|38=100|40=2|44=150.50|10=118|", false},
        {30, "35=D missing 54", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|38=100|40=2|44=150.50|10=118|", false},
        {31, "35=D missing 38", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|40=2|44=150.50|10=118|", false},
        {32, "35=D missing 40", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|38=100|44=150.50|10=118|", false},
        {33, "Invalid 54", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=9|38=100|40=2|44=150.50|10=118|", false},
        {34, "Invalid 38", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|38=-10|40=2|44=150.50|10=118|", false},
        {35, "Invalid 44", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|38=100|40=2|44=FREE|10=118|", false},
        {36, "Invalid 40", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|38=100|40=9|44=150.50|10=118|", false},
        {37, "40=1 without 44", "8=FIX.4.4|9=90|35=D|34=1|49=CLIENT|56=SERVER|52=20260925-16:46:00.000|11=ORDER1|55=AAPL|54=1|38=100|40=1|10=114|", true}, // Market order doesn't need price
        {38, "40=2 with 44", "8=FIX.4.4|9=100|35=D|34=1|49=CLIENT|56=SERVER|52=20260925-16:46:00.000|11=ORDER1|55=AAPL|54=1|38=100|40=2|44=150.50|10=106|", true}, // Limit order needs price
        {39, "40=2 without 44", "8=FIX.4.4|9=105|35=D|34=1|49=CLIENT|56=SERVER|11=ORDER1|55=AAPL|54=1|38=100|40=2|10=118|", false},
        {40, "Unknown 35", "8=FIX.4.4|9=105|35=ZZ|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {41, "Field-order violations", "8=FIX.4.4|9=105|34=1|35=D|49=CLIENT|56=SERVER|10=118|", false}, // Tag 35 must follow Tag 9
        {42, "Extremely large fields", "8=FIX.4.4|9=105|35=D|34=1|49=" + std::string(1000, 'A') + "|56=SERVER|10=118|", false},
        {43, "Extremely large BodyLength", "8=FIX.4.4|9=999999999|35=D|34=1|49=CLIENT|56=SERVER|10=118|", false},
        {44, "Random/fuzz input", "alksjd%#$!@#\x01\x029=35=D|==", false}
    };

    int passed_count = 0;
    for (const auto& test : tests) {
        if (run_test(test)) {
            passed_count++;
        }
    }

    std::cout << "\n----------------------------------------\n";
    std::cout << "Results: " << passed_count << " / " << tests.size() << " passed.\n";
    std::cout << "----------------------------------------\n";
}
