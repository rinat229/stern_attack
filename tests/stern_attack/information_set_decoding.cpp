#define BOOST_TEST_MODULE InformationSetDecoding
#define NUMBER_OF_SMALL_TEST_CASES 2
#define NUMBER_OF_MEDIUM_TEST_CASES 4

#include <algebra/binary_matrix.hpp>
#include <stern_attack/information_set_decoding.hpp>
#include "helper.hpp"

#include <boost/test/included/unit_test.hpp>
#include <algorithm>
#include <map>


BOOST_AUTO_TEST_CASE(InformationSetDecodingSmall) {
    std::map<int, int> numberOfErrors = {
        {1, 2},
        {2, 2},
    };

    for(int i = 1; i <= NUMBER_OF_SMALL_TEST_CASES; ++i){
        unsigned omega = numberOfErrors[i];
        std::string nameOfDirectory("small");
        nameOfDirectory += std::to_string(i) + "/";
        auto inputDataForMatrix = ReadLinesFromFile(nameOfDirectory + "check_matrix.txt");
        auto inputDataForSyndrome = ReadLinesFromFile(nameOfDirectory + "syndrome.txt");
        auto inputDataForErrorVector = ReadLinesFromFile(nameOfDirectory + "error_vector.txt");

        BOOST_TEST(inputDataForMatrix.size() == inputDataForSyndrome.front().size());
        BOOST_TEST(inputDataForMatrix.front().size() == inputDataForErrorVector.front().size());

        BinaryMatrix checkMatrix;
        for(auto& line: inputDataForMatrix) {
            std::reverse(line.begin(), line.end());
            checkMatrix.addRow(line);
        }

        std::reverse(inputDataForSyndrome.front().begin(), inputDataForSyndrome.front().end());
        boost::dynamic_bitset<> syndrome(inputDataForSyndrome.front());

        std::reverse(inputDataForErrorVector.front().begin(), inputDataForErrorVector.front().end());
        boost::dynamic_bitset<> errorVector(inputDataForErrorVector.front());
        boost::dynamic_bitset<> errorVectorFromISD = InformationSetDecoding(checkMatrix, syndrome, omega);

        BOOST_TEST(errorVector == errorVectorFromISD);
    }
}

BOOST_AUTO_TEST_CASE(InformationSetDecodingMedium) {
    std::map<int, int> numberOfErrors = {
        {1, 2},
        {2, 4},
        {3, 6},
        {4, 7},
    };

    for(int i = 1; i <= NUMBER_OF_MEDIUM_TEST_CASES; ++i){
        BOOST_CHECK(numberOfErrors.find(i) != numberOfErrors.end());

        unsigned omega = numberOfErrors[i];

        std::string nameOfDirectory("medium");
        nameOfDirectory += std::to_string(i) + "/";
        auto inputDataForMatrix = ReadLinesFromFile(nameOfDirectory + "check_matrix.txt");
        auto inputDataForSyndrome = ReadLinesFromFile(nameOfDirectory + "syndrome.txt");
        auto inputDataForErrorVector = ReadLinesFromFile(nameOfDirectory + "error_vector.txt");

        BOOST_TEST(inputDataForMatrix.size() == inputDataForSyndrome.front().size());
        BOOST_TEST(inputDataForMatrix.front().size() == inputDataForErrorVector.front().size());

        BinaryMatrix checkMatrix;
        for(auto& line: inputDataForMatrix) {
            std::reverse(line.begin(), line.end());
            checkMatrix.addRow(line);
        }

        std::reverse(inputDataForSyndrome.front().begin(), inputDataForSyndrome.front().end());
        boost::dynamic_bitset<> syndrome(inputDataForSyndrome.front());

        std::reverse(inputDataForErrorVector.front().begin(), inputDataForErrorVector.front().end());
        boost::dynamic_bitset<> errorVector(inputDataForErrorVector.front());
        boost::dynamic_bitset<> errorVectorFromISD = InformationSetDecoding(checkMatrix, syndrome, omega);

        BOOST_TEST(errorVector == errorVectorFromISD);
    }
}