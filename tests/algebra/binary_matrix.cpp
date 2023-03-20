#define BOOST_TEST_MODULE BinaryMatrixModule
#include <boost/test/included/unit_test.hpp>
#include <algebra/binary_matrix.hpp>

BOOST_AUTO_TEST_CASE(PermutationApplyingCheck1) {
    BinaryMatrix matrix = BinaryMatrix({std::string("010"), std::string("110")}).applyPermutation({2, 3, 1});
    BinaryMatrix permutedMatrix({std::string("100"), std::string("101")});

    BOOST_CHECK(matrix == permutedMatrix);    
}


BOOST_AUTO_TEST_CASE(SumOfColumns) {
    BinaryMatrix matrix = BinaryMatrix({
        std::string("010"),
        std::string("100"),
        std::string("111"),
        std::string("011"),
        std::string("011"),
    });

    BOOST_CHECK_EQUAL(matrix.sumOfColumns({0, 1, 2}), boost::dynamic_bitset<>(std::string("00111")));
    BOOST_CHECK_EQUAL(matrix.sumOfColumns({0, 2}), boost::dynamic_bitset<>(std::string("11010")));
    BOOST_CHECK_EQUAL(matrix.sumOfColumns({1}), boost::dynamic_bitset<>(std::string("11101")));
}