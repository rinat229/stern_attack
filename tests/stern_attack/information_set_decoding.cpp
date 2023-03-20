#define BOOST_TEST_MODULE PrimitivePermutationModule

#include <algebra/binary_matrix.hpp>
#include <stern_attack/information_set_decoding.hpp>
#include "helper.hpp"

#include <boost/test/included/unit_test.hpp>
#include <algorithm>

BOOST_AUTO_TEST_CASE(InformationSetDecodingSmall) {
    auto inputDataForMatrix = ReadLinesFromFile("check_matrix_small.txt");
    auto inputDataForSyndrome = ReadLinesFromFile("syndrome_small.txt");

    BOOST_TEST(inputDataForMatrix.front().size() == inputDataForSyndrome.front().size());

    BinaryMatrix checkMatrix;
    for(auto& line: inputDataForMatrix) {
        std::reverse(line.begin(), line.end());
        checkMatrix.addRow(line);
    }

    std::reverse(inputDataForSyndrome.front().begin(), inputDataForSyndrome.front().end());
    boost::dynamic_bitset<> syndrome(inputDataForSyndrome.front());
    
    unsigned omega = 2;

    InformationSetDecoding(checkMatrix, syndrome, omega);
}
