#pragma once

#include <vector>
#include <numeric>

class PermutationBase {
protected:
    std::vector<unsigned int> start_permutation;

    PermutationBase(unsigned int permSize) {
        start_permutation.resize(permSize);
        std::iota(start_permutation.begin(), start_permutation.end(), 0);
    }

    class PermutationBaseIterator {
    public:
        using ValueType = std::vector<unsigned int>;
        using PointerType = ValueType*;

        ValueType permutation;

        PermutationBaseIterator(const ValueType& permutation): permutation(permutation) {}

        PermutationBaseIterator(const ValueType&& permutation): permutation(std::move(permutation)) {}

        template <typename InputIteratorType>
        PermutationBaseIterator(ValueType::iterator begin, ValueType::iterator end): permutation(begin, end) {}

        ValueType& operator*() {
            return permutation;
        }

        PointerType operator->() {
            return &permutation;
        }
    };
};
