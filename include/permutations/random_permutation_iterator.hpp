#pragma once

#include "permutation_base.hpp"
#include <algorithm>
#include <random>


class RandomPermutation: public PermutationBase {
    unsigned changeableSize;

public:
    using PermutationBase::PermutationBase;

    RandomPermutation(unsigned permSize, unsigned changeableSize) : PermutationBase::PermutationBase(permSize), changeableSize(changeableSize) {}

    class RandomPermutationIterator: public PermutationBase::PermutationBaseIterator {
        unsigned changeableSize;
        unsigned leftPermsNumber = 1;
        bool rightPartWasChanged = true;
        value_type leftPermutation, tempContainer;
        std::mt19937 g;

    public:
        RandomPermutationIterator(const value_type& permutation, unsigned changeableSize): PermutationBase::PermutationBaseIterator(permutation), 
                                                                                            g(std::random_device()()), changeableSize(changeableSize),
                                                                                            leftPermutation(changeableSize), tempContainer(changeableSize)
        {
            std::iota(leftPermutation.begin(), leftPermutation.end(), 0);
        }

        RandomPermutationIterator& operator++() {
            if(leftPermsNumber == changeableSize){
                rightPartWasChanged = true;
                std::shuffle(permutation.begin(), permutation.end(), g);
                leftPermsNumber = 1;
            } else {
                rightPartWasChanged = false;
                std::shuffle(leftPermutation.begin(), leftPermutation.end(), g);
                std::copy(permutation.begin(), permutation.begin() + changeableSize, tempContainer.begin());

                for(unsigned idx = 0; idx < changeableSize; ++idx) {
                    permutation[idx] = tempContainer[leftPermutation[idx]];
                }
                // std::shuffle(permutation.begin(), permutation.begin() + changeableSize, g);
                ++leftPermsNumber;
            }

            return *this;
        }

        RandomPermutationIterator operator++(int) {
           RandomPermutationIterator temp = *this;
            ++(*this);

            return temp;
        }

        bool RightPartWasChanged() const {
            return rightPartWasChanged;
        }

        const value_type& GetLeftPermutation() const {
            return leftPermutation;
        }
    };

   RandomPermutationIterator begin() {
        return RandomPermutationIterator(start_permutation, changeableSize);
    }
};
