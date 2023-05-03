#pragma once

#include <string>
#include <vector>
#include <algorithm>



std::size_t NumberOfCombinations(int n, int k) {
    if (k > n) return 0;
    if (k * 2 > n) k = n - k;
    if (k == 0) return 1;

    std::size_t result = n;
    for(int i = 2; i <= k; ++i) {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}


class Combination {
    int n, k;

public:
    /**
     * Generating iterator for k-combinations of n
     * @param k size of combination
     * @param n size of set, i.e. [0...n-1]
     */
    Combination(int k, int n): k(k), n(n) {} 

    std::size_t GetNumberOfCombinations() const {
        return NumberOfCombinations(n, k);
    }

    class CombinationIterator {
    public:
        using value_type = std::vector<unsigned>;
        value_type combination;
        std::string bitmask;
        bool canBePermuted = true;


        CombinationIterator(const std::string& _bitmask): bitmask(_bitmask) {
            combination.resize(std::count(bitmask.begin(), bitmask.end(), 1));

            GenerateCombinationFromBitmask();
        }

        value_type& operator*() {
            return combination;
        }
;
        value_type* operator->() {
            return &combination;
        }

        CombinationIterator& operator++() {
            canBePermuted = std::prev_permutation(bitmask.begin(), bitmask.end());
            GenerateCombinationFromBitmask();

            return *this;
        }

        CombinationIterator operator++(int) {
            auto tempIter = *this;
            ++(*this);

            return tempIter;
        }

        bool CombinationsStillExist() {
            return canBePermuted;
        }
    
    private:
        void GenerateCombinationFromBitmask() {
            for(size_t idxBitmask = 0, idxCombination = 0; idxBitmask < bitmask.size(); ++idxBitmask){
                if(bitmask[idxBitmask]){
                    combination[idxCombination++] = idxBitmask;
                }
            }
        }
    };

    CombinationIterator begin(){
        std::string bitmask(k, 1);
        bitmask.resize(n, 0);

        return CombinationIterator(bitmask);
    }
};