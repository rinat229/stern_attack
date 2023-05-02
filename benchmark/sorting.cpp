#include <algorithm>
#include <vector>
#include <random>
#include <string>
#include <thread>


#include <boost/dynamic_bitset.hpp>
#include <boost/sort/sort.hpp>

#include <utils/radixsort.hpp>
#include <utils/benchmark.hpp>


std::vector<boost::dynamic_bitset<>> GenerateRandomVec(unsigned length, unsigned bitsetLen){
    std::vector<boost::dynamic_bitset<>> vec;
    vec.reserve(length);

    std::random_device rd;
    std::mt19937 gen( rd());
    std::bernoulli_distribution d(0.5);

    for(int i = 0; i < length; ++i) {
        boost::dynamic_bitset<> row(bitsetLen);
        for(int k = 0; k < bitsetLen; ++k) {
            row[k] = d(gen);
        }
        vec.push_back(row);
    }

    return vec;
}


void BenchmarkAlgorithm(auto algorithm, std::vector<boost::dynamic_bitset<>> data, const std::string& name) {
    Timer timer(name);

    algorithm(data.begin(), data.end());
}


int main() {
    int length = 1000000, bitsetLen = 28;
    auto randomSet = GenerateRandomVec(length, bitsetLen);

    auto cmp = [](auto &lhs, auto& rhs) {
        return lhs < rhs;
    };

    using IteratorType = decltype(randomSet.begin());
    using CompareType = decltype(cmp);

    BenchmarkAlgorithm(std::sort<IteratorType>, randomSet, "std::sort");

    auto radixsort = std::bind(Radixsort<IteratorType>, std::placeholders::_1, std::placeholders::_2, std::identity());
    BenchmarkAlgorithm(radixsort, randomSet, "Radixsort");
    
    auto blockIndirectSort = std::bind(boost::sort::block_indirect_sort<IteratorType, CompareType, nullptr>, std::placeholders::_1, std::placeholders::_2, cmp);
    BenchmarkAlgorithm(blockIndirectSort, randomSet, "boost::sort::block_indirect_sort(" + std::to_string(std::thread::hardware_concurrency()) + ')');

    auto pdqsort = std::bind(boost::sort::pdqsort<IteratorType, CompareType>, std::placeholders::_1, std::placeholders::_2, cmp);
    BenchmarkAlgorithm(pdqsort, randomSet, "boost::sort::pdqsort");

    auto spinsort = std::bind(boost::sort::spinsort<IteratorType, CompareType>, std::placeholders::_1, std::placeholders::_2, cmp);
    BenchmarkAlgorithm(spinsort, randomSet, "boost::sort::spinsort");

    auto sample_sort = std::bind(boost::sort::sample_sort<IteratorType, CompareType, nullptr>, std::placeholders::_1, std::placeholders::_2, cmp);
    BenchmarkAlgorithm(sample_sort, randomSet, "boost::sort::sample_sort");

}