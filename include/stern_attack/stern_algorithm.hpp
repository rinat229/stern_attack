#pragma once

#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>

#include <algebra/binary_matrix.hpp>
#include <permutations/combination_iterator.hpp>
#include <permutations/primitive_permutation_iterator.hpp>
#include <permutations/random_permutation_iterator.hpp>
#include "base_decoding.hpp"

/**
 * Makes one step of Stern algorithm to already permuted check matrix
 * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
 * @param syndrome syndrome vector 
 * @param p number of columns that should be summed
 * @param omega number of errors in codeword (i.e. number of ones in error vector)
 * @return filled std::optional<boost::dynamic_bitset<>> if success, else empty optional
 */
std::optional<boost::dynamic_bitset<>> SternAlgorithmStep(BinaryMatrix& checkMatrix, boost::dynamic_bitset<> syndrome, const unsigned p, const unsigned omega, const unsigned l) {
    if(!GaussElimination(checkMatrix, syndrome)){
        return std::optional<boost::dynamic_bitset<>>();
    }

    unsigned rows = checkMatrix.RowsSize();
    unsigned cols = checkMatrix.ColumnsSize();
    unsigned colsSizeOfQ = (cols - rows) / 2;
    std::vector<std::pair<boost::dynamic_bitset<>, std::vector<unsigned>>> projectedSum1, projectedSum2;
    boost::dynamic_bitset<> projectedSyndrome = Projection(syndrome, l);

    for(auto combinationIter = Combination(p, colsSizeOfQ).begin(); combinationIter.CombinationsStillExist(); ++combinationIter) {
        projectedSum1.emplace_back(checkMatrix.sumOfColumns(*combinationIter, std::make_optional(l)), *combinationIter);

        std::vector<unsigned> shiftedCombination = *combinationIter;
        std::for_each(shiftedCombination.begin(), shiftedCombination.end(), [&colsSizeOfQ](auto& element){
            element += colsSizeOfQ;
        });

        projectedSum2.emplace_back(checkMatrix.sumOfColumns(shiftedCombination, std::make_optional(l)) ^ projectedSyndrome, shiftedCombination);
    }

    std::sort(projectedSum1.begin(), projectedSum1.end());
    std::sort(projectedSum2.begin(), projectedSum2.end());

    for(auto iter1 = projectedSum1.begin(), iter2 = projectedSum2.begin(); iter1 != projectedSum1.end() && iter2 != projectedSum2.end();) {
        if(iter1->first < iter2->first) {
            ++iter1;
        } else if(iter1->first > iter2->first) {
            ++iter2;
        } else {
            if((checkMatrix.sumOfColumns(iter1->second) ^ checkMatrix.sumOfColumns(iter2->second) ^ syndrome).count() == omega - 2 * p) {
                std::cout << (checkMatrix.sumOfColumns(iter1->second, std::optional(l)) ^ checkMatrix.sumOfColumns(iter2->second, std::optional(l)) ^ projectedSyndrome) << '\n';
                boost::dynamic_bitset<> errorVector(cols);

                for(unsigned setBitIdx = 0; setBitIdx < p; ++setBitIdx) {
                    errorVector.set(iter1->second[setBitIdx]);
                }

                for(unsigned setBitIdx = 0; setBitIdx < p; ++setBitIdx) {
                    errorVector.set(iter2->second[setBitIdx]);
                }

                const auto matchedSupp = checkMatrix.sumOfColumns(iter1->second) ^ checkMatrix.sumOfColumns(iter2->second) ^ syndrome;
                for(boost::dynamic_bitset<>::size_type setBitIdx = 0; setBitIdx < matchedSupp.size(); ++setBitIdx) {
                    if(matchedSupp[setBitIdx]){
                        errorVector.set(setBitIdx + colsSizeOfQ);
                    }
                }

                return std::optional<boost::dynamic_bitset<>>(errorVector);
            } else {
                ++iter1;
                ++iter2;
            }
        }
    }

    return std::optional<boost::dynamic_bitset<>>();
}
