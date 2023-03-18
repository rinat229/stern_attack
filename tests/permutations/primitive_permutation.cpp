#define BOOST_TEST_MODULE PrimitivePermutationModule
#include <boost/test/included/unit_test.hpp>
#include <permutations/primitive_permutation_iterator.hpp>
#include "helper.hpp"

#include <vector>


using PermContainerType = PrimitivePermutation::PrimitivePermutationIterator::value_type;

BOOST_AUTO_TEST_CASE(PrimitivePermutationCheck) {
    auto iter = PrimitivePermutation(3).begin();

    BOOST_CHECK_EQUAL(*(iter++), PermContainerType({0, 1, 2}));
    BOOST_CHECK_EQUAL(*(iter++), PermContainerType({0, 2, 1}));
    BOOST_CHECK_EQUAL(*(iter++), PermContainerType({1, 0, 2}));
}

BOOST_AUTO_TEST_CASE(IteratingPermutations) {
    std::vector<std::vector<unsigned>> perms = {
        {0, 1, 2},
        {0, 2, 1},
        {1, 0, 2},
        {1, 2, 0},
        {2, 0, 1},
        {2, 1, 0}
    };
    std::size_t index = 0;
    for(auto iter = PrimitivePermutation(3).begin(); iter.CanBePermuted(); ++iter){
        std::cout << *iter;
        BOOST_CHECK_EQUAL(*iter, perms[index++]);
    }
}