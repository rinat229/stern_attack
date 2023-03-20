#define BOOST_TEST_MODULE PrimitivePermutationModule
#define NUMBER_OF_SMALL_TEST_CASES 2

#include <algebra/binary_matrix.hpp>
#include <stern_attack/information_set_decoding.hpp>
#include "helper.hpp"

#include <boost/test/included/unit_test.hpp>
#include <algorithm>

BOOST_AUTO_TEST_CASE(InformationSetDecodingSmall) {
    unsigned omega = 2;

    for(int i = 1; i <= NUMBER_OF_SMALL_TEST_CASES; ++i){
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
