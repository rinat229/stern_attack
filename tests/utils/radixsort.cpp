#define BOOST_TEST_MODULE RadixSort
#include <boost/test/included/unit_test.hpp>
#include <boost/dynamic_bitset.hpp>
#include <utils/radixsort.hpp>
#include <random>
#include <algorithm>
#include <iostream>


// namespace std{
//     template <typename CharT, typename Traits, typename ValueType>
//     std::basic_ostream<CharT, Traits>&
//     operator<<(std::basic_ostream<CharT, Traits>& os, const std::vector<ValueType>& b){
//         for(const auto& value: b){
//             os << value << ' ';
//         }

//         return os;
//     }
// }

std::vector<boost::dynamic_bitset<>> GenerateRandomVec(unsigned numberOfSets, unsigned bitsetLen){
    std::vector<boost::dynamic_bitset<>> vec;
    vec.reserve(numberOfSets);

    std::random_device rd;
    std::mt19937 gen( rd());
    std::bernoulli_distribution d(0.5);

    for(int i = 0; i < numberOfSets; ++i) {
        boost::dynamic_bitset<> row(bitsetLen);
        for(int k = 0; k < bitsetLen; ++k) {
            row[k] = d(gen);
        }
        vec.push_back(row);
    }

    return vec;
}

BOOST_AUTO_TEST_CASE(RadixSort1) {
    unsigned numOfSets = 10, bitsetLen = 6;
    std::vector<boost::dynamic_bitset<>> vec = GenerateRandomVec(numOfSets, bitsetLen);
    std::vector<boost::dynamic_bitset<>> vecShouldBe = vec;

    std::cout << vec << '\n';

    std::sort(vecShouldBe.begin(), vecShouldBe.end());
    Radixsort(vec.begin(), vec.end());

    std::cout << vecShouldBe << '\n';
    std::cout << vec << '\n';

    BOOST_TEST(vec == vecShouldBe);
    BOOST_TEST(boost::dynamic_bitset<>(std::string("01101")) > boost::dynamic_bitset<>(std::string("01001")));
}

BOOST_AUTO_TEST_CASE(RadixSort2) {
    std::vector<std::pair<boost::dynamic_bitset<>, std::vector<unsigned>>> vec;
    vec.emplace_back(std::string("1101"), std::vector<unsigned>{3, 5, 6});
    Radixsort(vec.begin(), vec.end(), [](auto& pair){
        return pair.first;
    });
}