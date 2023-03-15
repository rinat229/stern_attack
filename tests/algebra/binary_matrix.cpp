#define BOOST_TEST_MODULE BinaryMatrixModule
#include <boost/test/included/unit_test.hpp>
#include <algebra/binary_matrix.hpp>
#include <iostream>

BOOST_AUTO_TEST_CASE(PermutationCheck1) {
    BinaryMatrix matrix = BinaryMatrix({std::string("010"), std::string("110")}).applyPermutation({2, 3, 1});
    BinaryMatrix permutedMatrix({std::string("100"), std::string("101")});

    BOOST_CHECK(matrix == permutedMatrix);    
}