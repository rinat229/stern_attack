#pragma once

#include <vector>
#include <numeric>

class PermutationBase {
public:
    std::vector<unsigned> start_permutation;

    PermutationBase(unsigned permSize) {
        start_permutation.resize(permSize);
        std::iota(start_permutation.begin(), start_permutation.end(), 0);
    }

    class PermutationBaseIterator {
    public:
        using value_type = std::vector<unsigned>;
        using pointer_type = value_type*;

        value_type permutation;
        bool notEndYet = true;

        PermutationBaseIterator(const value_type& permutation): permutation(permutation) {}

        PermutationBaseIterator(const value_type&& permutation): permutation(std::move(permutation)) {}

        template <typename InputIterator, typename = std::enable_if<std::is_same<typename InputIterator::value_type, unsigned>::value || 
                                                                    std::is_same<typename InputIterator::value_type, int>::value> >
        PermutationBaseIterator(InputIterator begin, InputIterator end): permutation(begin, end) {}

        value_type& operator*() {
            return permutation;
        }

        pointer_type operator->() {
            return &permutation;
        }

        bool operator==(const PermutationBaseIterator& other){
            return other.permutation == permutation;
        }

        bool operator!=(const PermutationBaseIterator& other){
            return other.permutation != permutation;
        }

        bool CanBePermuted(){
            return notEndYet;
        }
    };
};
