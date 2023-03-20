#include <boost/test/included/unit_test.hpp>
#include <string>
#include <vector>
#include <filesystem>

#define STR(x) #x
#define RETURN_STR_MACRO(x) STR(x)


std::vector<std::string> ReadLinesFromFile(const std::string& file) {
    std::string binaryTestData = RETURN_STR_MACRO(TEST_DATA) + std::string("/") + file;
    BOOST_TEST(std::filesystem::exists(binaryTestData));

    std::ifstream ifs(binaryTestData);
    std::vector<std::string> lines;
    std::string line;

    while(std::getline(ifs, line)) {
        lines.push_back(line);
    }

    return lines;
}
