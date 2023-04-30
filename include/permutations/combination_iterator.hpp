#pragma once

#include <string>
#include <vector>
#include <algorithm>


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
        unsigned long long res = 1;
        
        for(int i = k; i <= n; ++i) {
            res *= i;
        }

        for(int i = 1; i <= n - k; ++i) {
            res /= i;
        }

        return res;
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