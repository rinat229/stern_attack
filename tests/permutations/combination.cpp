#define BOOST_TEST_MODULE PrimitivePermutationModule
#include <boost/test/included/unit_test.hpp>
#include <permutations/combination_iterator.hpp>
#include "helper.hpp"

#include <vector>


BOOST_AUTO_TEST_CASE(CombinationTest) {
    Combination comb(2, 4);
    unsigned idxCombination = 0;
    std::vector<std::vector<unsigned>> allCombinations{
        {0, 1},
        {0, 2},
        {0, 3},
        {1, 2},
        {1, 3},
        {2, 3}
    };

    for(auto iter = comb.begin(); iter.CombinationsStillExist(); ++iter){
        std::cout << *iter << '\n';
        BOOST_CHECK_EQUAL(allCombinations[idxCombination++], *iter);
    }
}

BOOST_AUTO_TEST_CASE(NumberOfCombs) {
    BOOST_CHECK_EQUAL(NumberOfCombinations(134, 1), 134);
}