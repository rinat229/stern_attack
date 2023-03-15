#define BOOST_TEST_MODULE PrimitivePermutationModule
#include <boost/test/included/unit_test.hpp>
#include <permutations/primitive_permutation_iterator.hpp>
#include "helper.hpp"


using PermContainerType = PrimitivePermutation::PrimitivePermutationIterator::value_type;

BOOST_AUTO_TEST_CASE(PrimitivePermutationCheck) {
    auto iter = PrimitivePermutation(3).begin();

    BOOST_CHECK_EQUAL(*(iter++), PermContainerType({0, 1, 2}));
    BOOST_CHECK_EQUAL(*(iter++), PermContainerType({0, 2, 1}));
    BOOST_CHECK_EQUAL(*(iter++), PermContainerType({1, 0, 2}));

    BOOST_CHECK_EQUAL(*PrimitivePermutation(3).end(), PermContainerType({2, 1, 0}));
}