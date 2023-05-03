#include <algorithm>
#include <vector>
#include <random>
#include <string>
#include <thread>
#include <execution>
#include <cassert>

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

void BenchmarkAlgorithm(auto algorithm, std::vector<boost::dynamic_bitset<>> data, const std::string& name, const auto& dataToCompare) {
    {
        Timer timer(name);

        algorithm(data.begin(), data.end());
    }

    if(data != dataToCompare){
        std::cerr << name << " is sorted wrong\n";
    }
}

void BenchmarkBIS(std::vector<boost::dynamic_bitset<>> data, const std::string& name, const unsigned threadsNum, const auto& dataToCompare) {
    {
        Timer timer(name);

        boost::sort::block_indirect_sort(data.begin(), data.end(), threadsNum);
    }

    if(data != dataToCompare){
        std::cerr << name << " is sorted wrong\n";
    }
}


void BenchmarkStdSort(std::vector<boost::dynamic_bitset<>> data, const std::string& name, auto executionPolicy, const auto& dataToCompare) {
    {
        Timer timer(name);

        std::sort(executionPolicy, data.begin(), data.end());
    }

    if(data != dataToCompare){
        std::cerr << name << " is sorted wrong\n";
    }
}


int main() {
    int length = 1000000, bitsetLen = 28;
    auto randomSet = GenerateRandomVec(length, bitsetLen);
    auto setToCompare = randomSet;

    std::ranges::sort(setToCompare);

    auto cmp = [](auto &lhs, auto& rhs) {
        // return lhs.to_ulong() < rhs.to_ulong();
        return lhs < rhs;
    };

    using IteratorType = decltype(randomSet.begin());
    using CompareType = decltype(cmp);

    BenchmarkAlgorithm(std::sort<IteratorType>, randomSet, "std::sort", setToCompare);
    BenchmarkAlgorithm(std::sort_heap<IteratorType>, randomSet, "std::sort_heap", setToCompare);                          

    BenchmarkStdSort(randomSet, "std::sort(std::unseq)", std::execution::unseq, setToCompare);
    BenchmarkStdSort(randomSet, "std::sort(std::par)", std::execution::par, setToCompare);
    BenchmarkStdSort(randomSet, "std::sort(std::par_unseq)", std::execution::par_unseq, setToCompare);

    auto radixsort = std::bind(Radixsort<IteratorType>, std::placeholders::_1, std::placeholders::_2, std::identity());
    BenchmarkAlgorithm(radixsort, randomSet, "Radixsort", setToCompare);

    BenchmarkBIS(randomSet, "boost::sort::block_indirect_sort(" + std::to_string(std::thread::hardware_concurrency()) + ')', std::thread::hardware_concurrency(), setToCompare);
    BenchmarkBIS(randomSet, "boost::sort::block_indirect_sort(1)", 1, setToCompare);
    BenchmarkBIS(randomSet, "boost::sort::block_indirect_sort(2)", 2, setToCompare);
    BenchmarkBIS(randomSet, "boost::sort::block_indirect_sort(8)", 8, setToCompare);

    auto pdqsort = std::bind(boost::sort::pdqsort<IteratorType, CompareType>, std::placeholders::_1, std::placeholders::_2, cmp);
    BenchmarkAlgorithm(pdqsort, randomSet, "boost::sort::pdqsort", setToCompare);

    auto pdqsortBranchless = std::bind(boost::sort::pdqsort_branchless<IteratorType, CompareType>, std::placeholders::_1, std::placeholders::_2, cmp);
    BenchmarkAlgorithm(pdqsortBranchless, randomSet, "boost::sort::pdqsort_branchless", setToCompare);

    auto spinsort = std::bind(boost::sort::spinsort<IteratorType, CompareType>, std::placeholders::_1, std::placeholders::_2, cmp);
    BenchmarkAlgorithm(spinsort, randomSet, "boost::sort::spinsort", setToCompare);

    auto samplesort = [&setToCompare, &cmp](std::vector<boost::dynamic_bitset<>> data, unsigned numThreads){
        {
            Timer timer(std::string("boost::sample_sort") + std::to_string(numThreads));
            boost::sort::sample_sort(data.begin(), data.end(), cmp, numThreads);
        }

        if(data != setToCompare){
            std::cerr << "boost::sample_sort is sorted wrong\n";
        }
    };

    samplesort(randomSet, 1);
    samplesort(randomSet, 2);
    samplesort(randomSet, 4);
    samplesort(randomSet, 10);
}