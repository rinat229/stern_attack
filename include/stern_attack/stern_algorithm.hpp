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
#include <utils/cartesian_product.hpp>
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
    unsigned colsSizeOfQ = cols - rows;
    unsigned halfColsSizeOfQ = colsSizeOfQ / 2;
    using CollisionType = std::pair<boost::dynamic_bitset<>, std::vector<unsigned>>;

    std::vector<CollisionType> projectedSum1, projectedSum2;
    boost::dynamic_bitset<> projectedSyndrome = Projection(syndrome, l);

    for(auto combinationIter = Combination(p, halfColsSizeOfQ).begin(); combinationIter.CombinationsStillExist(); ++combinationIter) {
        projectedSum1.emplace_back(checkMatrix.sumOfColumns(*combinationIter, std::make_optional(l)), *combinationIter);

        std::vector<unsigned> shiftedCombination = *combinationIter;
        std::for_each(shiftedCombination.begin(), shiftedCombination.end(), [&halfColsSizeOfQ](auto& element){
            element += halfColsSizeOfQ;
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
            auto iterEnd1 = std::find_if(iter1, projectedSum1.end(), [&iter1] (auto &iter) {
                return iter.first != iter1->first;
            });

            auto iterEnd2 = std::find_if(iter2, projectedSum2.end(), [&iter2] (auto &iter) {
                return iter.first != iter2->first;
            });

            std::vector<std::tuple<CollisionType, CollisionType>> matchedResults;
            cartesian_product(
                std::back_inserter(matchedResults),
                std::make_pair(iter1, iterEnd1),
                std::make_pair(iter2, iterEnd2)
            );

            for(const auto&[iter1, iter2] : matchedResults){
                if((checkMatrix.sumOfColumns(iter1.second) ^ checkMatrix.sumOfColumns(iter2.second) ^ syndrome).count() == omega - 2 * p) {
                    boost::dynamic_bitset<> errorVector(cols);

                    for(unsigned setBitIdx = 0; setBitIdx < p; ++setBitIdx) {
                        errorVector.set(iter1.second[setBitIdx]);
                        errorVector.set(iter2.second[setBitIdx]);
                    }

                    const auto matchedSupp = checkMatrix.sumOfColumns(iter1.second) ^ checkMatrix.sumOfColumns(iter2.second) ^ syndrome;
                    for(boost::dynamic_bitset<>::size_type setBitIdx = 0; setBitIdx < matchedSupp.size(); ++setBitIdx) {
                        if(matchedSupp[setBitIdx]){
                            errorVector.set(setBitIdx + colsSizeOfQ);
                        }
                    }

                    return std::optional<boost::dynamic_bitset<>>(errorVector);
                }
            }

            iter1 = iterEnd1;
            iter2 = iterEnd2;
        }
    }

    return std::optional<boost::dynamic_bitset<>>();
}
