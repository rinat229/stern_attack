#pragma once

#include "permutation_base.hpp"
#include <algorithm>


class PrimitivePermutation: public PermutationBase {
public:
    PrimitivePermutation(int permSize): PermutationBase(permSize) {};

    class PrimitivePermutationIterator: public PermutationBase::PermutationBaseIterator {
    public:
        PrimitivePermutationIterator& operator++() {
            std::next_permutation(permutation.begin(), permutation.end());

            return *this;
        }

        PrimitivePermutationIterator operator++(int) {
            PrimitivePermutationIterator temp = *this;
            ++(*this);

            return temp;
        }

        PrimitivePermutationIterator& operator--() {
            std::prev_permutation(permutation.begin(), permutation.end());

            return *this;
        }

        PrimitivePermutationIterator operator--(int) {
            PrimitivePermutationIterator temp = *this;
            --(*this);

            return temp;
        }
    };

    PrimitivePermutationIterator begin() {
        return PrimitivePermutationIterator(start_permutation);
    }

    PrimitivePermutationIterator end() {
        return PrimitivePermutationIterator(start_permutation.rbegin(), start_permutation.rend());
    }
};  