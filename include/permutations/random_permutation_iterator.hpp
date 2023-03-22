#pragma once

#include "permutation_base.hpp"
#include <algorithm>
#include <random>


class RandomPermutation: public PermutationBase {

public:
    using PermutationBase::PermutationBase;

    class RandomPermutationIterator: public PermutationBase::PermutationBaseIterator {
        std::mt19937 g;
    public:
        RandomPermutationIterator(const value_type& permutation): PermutationBase::PermutationBaseIterator(permutation), g(std::random_device()()) {}

        RandomPermutationIterator& operator++() {
            std::shuffle(permutation.begin(), permutation.end(), g);

            return *this;
        }

        RandomPermutationIterator operator++(int) {
           RandomPermutationIterator temp = *this;
            ++(*this);

            return temp;
        }
    };

   RandomPermutationIterator begin() {
        return RandomPermutationIterator(start_permutation);
    }
};  