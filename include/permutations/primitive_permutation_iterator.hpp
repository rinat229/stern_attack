#pragma once

#include "permutation_base.hpp"
#include <algorithm>


class PrimitivePermutation: public PermutationBase {
public:
    using PermutationBase::PermutationBase;

    class PrimitivePermutationIterator: public PermutationBase::PermutationBaseIterator {
    public:
        using PermutationBase::PermutationBaseIterator::PermutationBaseIterator;

        PrimitivePermutationIterator& operator++() {
            if(!std::next_permutation(permutation.begin(), permutation.end())){
                notEndYet = false;
            }

            return *this;
        }

        PrimitivePermutationIterator operator++(int) {
            PrimitivePermutationIterator temp = *this;
            ++(*this);

            return temp;
        }
    };

    PrimitivePermutationIterator begin() {
        return PrimitivePermutationIterator(start_permutation);
    }
};  