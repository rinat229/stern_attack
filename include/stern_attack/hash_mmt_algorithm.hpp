#pragma once

#include <boost/dynamic_bitset.hpp>
#include <boost/sort/sort.hpp>

#include <iostream>
#include <algorithm>
#include <cassert>
#include <execution>
#include <numeric>
#include <tuple>
#include <vector>
#include <unordered_map>

#include <algebra/binary_matrix.hpp>
#include <permutations/combination_iterator.hpp>
#include <permutations/primitive_permutation_iterator.hpp>
#include <permutations/random_permutation_iterator.hpp>
#include "base_decoding.hpp"
#include "mmt_algorithm.hpp"


class MMTHashAlgorithm : public MMTAlgorithm {
public:
    /**
     * Creates an options object for MMT algorithm which uses unordered_map 
     * for looking collisions 
     * @param cols number of columns in matrix, p initializes as 0.006 * cols (it's optimal parameter)
     * l1 as 0.028 * cols, l2 as 0.006 * cols
    */
    MMTHashAlgorithm(unsigned cols, unsigned rows) : MMTAlgorithm(cols, rows) {}

    /**
     * Makes one step of information-set decoding to already permuted check matrix
     * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
     * @param syndrome syndrome vector 
     * @param omega number of errors in codeword (i.e. number of ones in error vector)
     * @return filled std::optional<boost::dynamic_bitset<>> if success, else empty optional
     */
    std::optional<boost::dynamic_bitset<>> operator()(BinaryMatrix& checkMatrix, boost::dynamic_bitset<> syndrome, const unsigned omega) const {
        if(!GaussElimination(checkMatrix, syndrome)) {
            return std::optional<boost::dynamic_bitset<>>();
        }

        unsigned rows = checkMatrix.RowsSize();
        unsigned cols = checkMatrix.ColumnsSize();
        unsigned colsSizeOfQ = cols - rows + l;

        unsigned halfColsSizeOfQ = colsSizeOfQ / 2;

        auto combination = Combination(p, halfColsSizeOfQ);
        auto numberOfCombs = combination.GetNumberOfCombinations();

        std::vector<CollisionType> projectedSum1;
        std::unordered_multimap<BinaryMatrix::BitContainerType, IndexType> projectedSum2;

        std::vector<CollisionType> /*projectedSum11,*/ projectedSum12;
        std::vector<CollisionType> projectedSum21, projectedSum22;

        projectedSum12.reserve(expectedLengthLevel1);
        projectedSum21.reserve(expectedLengthLevel1);
        projectedSum22.reserve(expectedLengthLevel1);

        projectedSum1.reserve(expectedLengthLevel2);
        projectedSum2.reserve(expectedLengthLevel2);

        boost::dynamic_bitset<> projectedSyndrome1 = Projection(syndrome, l1);
        boost::dynamic_bitset<> projectedSyndrome2 = Projection(syndrome, l, l1);

        BinaryMatrix checkMatrixTr = checkMatrix.TransposeMatrix(0, checkMatrix.RowsSize());
        BinaryMatrix checkMatrixOnL1 = checkMatrix.TransposeMatrix(0, l1);
        BinaryMatrix checkMatrixOnL2 = checkMatrix.TransposeMatrix(l1, l);

        for(auto combinationIter = combination.begin(); combinationIter.CombinationsStillExist(); ++combinationIter) {
            /// TODO: projectedSum11 == projectedSum21 ???
            // projectedSum11.emplace_back(checkMatrix.sumOfColumns(*combinationIter, l1, l), *combinationIter);
            projectedSum21.emplace_back(checkMatrix.sumOfColumns(*combinationIter, l1, l), *combinationIter);

            std::vector<unsigned> shiftedCombination = *combinationIter;
            std::for_each(shiftedCombination.begin(), shiftedCombination.end(), [&halfColsSizeOfQ](auto& element){
                element += halfColsSizeOfQ;
            });

            projectedSum12.emplace_back(checkMatrixOnL2.sumOfRows(shiftedCombination), shiftedCombination);
            projectedSum22.emplace_back(checkMatrixOnL2.sumOfRows(shiftedCombination) ^ projectedSyndrome2, shiftedCombination);
        }

        auto compare = [](auto &a, auto& b) {
            return a.first.to_ulong() < b.first.to_ulong();
        };

        boost::sort::pdqsort_branchless(projectedSum12.begin(), projectedSum12.end(), compare);
        boost::sort::pdqsort_branchless(projectedSum21.begin(), projectedSum21.end(), compare);
        boost::sort::pdqsort_branchless(projectedSum22.begin(), projectedSum22.end(), compare);

        for(auto iter1 = projectedSum12.begin(), iter2 = projectedSum12.begin(); iter1 != projectedSum12.end() && iter2 != projectedSum12.end();) {
            if(iter1->first < iter2->first){
                ++iter1;
            } else if(iter1->first > iter2->first) {
                ++iter2;
            } else {
                auto iterEnd1 = std::find_if(iter1, projectedSum12.end(), [&iter1] (auto &iter) {
                    return iter.first != iter1->first;
                });

                auto iterEnd2 = std::find_if(iter2, projectedSum12.end(), [&iter2] (auto &iter) {
                    return iter.first != iter2->first;
                });

                for(auto iterBegin1 = iter1; iterBegin1 != iterEnd1; ++iterBegin1){
                    for(auto iterBegin2 = iter2; iterBegin2 != iterEnd2; ++iterBegin2){
                        if(iterBegin1->first == iterBegin2->first) {
                            std::vector<unsigned> indexUnion;
                            indexUnion.reserve(iterBegin1->second.size() + iterBegin2->second.size());
                            indexUnion.insert(indexUnion.end(), iterBegin1->second.begin(), iterBegin1->second.end());
                            indexUnion.insert(indexUnion.end(), iterBegin2->second.begin(), iterBegin2->second.end());

                            /// TODO: is it a need in recomputation of sum of columns???
                            projectedSum1.emplace_back(checkMatrixOnL1.sumOfRows(indexUnion), indexUnion);
                        }
                    }
                }

                if(projectedSum1.size() > expectedLengthLevel2) {
                    break;
                }

                iter1 = iterEnd1;
                iter2 = iterEnd2;
            }
        }

        for(auto iter1 = projectedSum21.begin(), iter2 = projectedSum22.begin(); iter1 != projectedSum21.end() && iter2 != projectedSum22.end();) {
            if(iter1->first < iter2->first){
                ++iter1;
            } else if(iter1->first > iter2->first) {
                ++iter2;
            } else {
                auto iterEnd1 = std::find_if(iter1, projectedSum21.end(), [&iter1] (auto &iter) {
                    return iter.first != iter1->first;
                });

                auto iterEnd2 = std::find_if(iter2, projectedSum22.end(), [&iter2] (auto &iter) {
                    return iter.first != iter2->first;
                });

                for(auto iterBegin1 = iter1; iterBegin1 != iterEnd1; ++iterBegin1){
                    for(auto iterBegin2 = iter2; iterBegin2 != iterEnd2; ++iterBegin2){
                        if(iterBegin1->first == iterBegin2->first) {
                            std::vector<unsigned> indexUnion;
                            indexUnion.reserve(iterBegin1->second.size() + iterBegin2->second.size());
                            indexUnion.insert(indexUnion.end(), iterBegin1->second.begin(), iterBegin1->second.end());
                            indexUnion.insert(indexUnion.end(), iterBegin2->second.begin(), iterBegin2->second.end());

                            /// TODO: is it a need in recomputation of sum of columns???
                            projectedSum2.emplace(checkMatrixOnL1.sumOfRows(indexUnion) ^ projectedSyndrome1, indexUnion);
                        }
                    }
                }

                if(projectedSum2.size() > expectedLengthLevel2) {
                    break;
                }

                iter1 = iterEnd1;
                iter2 = iterEnd2;
            }
        }

        lengthsL2.push_back(projectedSum1.size());
        lengthsL2.push_back(projectedSum2.size());
        // indexes of columns should guarantee to be sorted for finding symmetric difference
        std::for_each(projectedSum1.begin(), projectedSum1.end(), [](CollisionType& element) {
            std::sort(element.second.begin(), element.second.end());
        });

        std::for_each(projectedSum2.begin(), projectedSum2.end(), [](CollisionType element) {
            std::sort(element.second.begin(), element.second.end());
        });

        for(auto &[projectedOnL1Bitset, indexSet1] : projectedSum1) {
            auto [iterBegin, iterEnd] = projectedSum2.equal_range(projectedOnL1Bitset);

            for(auto iterMatched = iterBegin; iterMatched != iterEnd; ++iterMatched){
                std::vector<unsigned> indexColumns;
                indexColumns.reserve(indexSet1.size() + iterMatched->second.size());

                std::set_symmetric_difference(indexSet1.begin(), indexSet1.end(),
                                            iterMatched->second.begin(), iterMatched->second.end(),
                                            std::back_inserter(indexColumns));

                if((checkMatrixTr.sumOfRows(indexColumns) ^ syndrome).count() == omega - indexColumns.size()) {
                    boost::dynamic_bitset<> errorVector(cols);

                    for(const auto& idx : indexColumns) {
                        errorVector.set(idx);
                    }

                    const auto matchedSupp = checkMatrix.sumOfColumns(indexColumns) ^ syndrome;
                    for(boost::dynamic_bitset<>::size_type setBitIdx = 0; setBitIdx < matchedSupp.size(); ++setBitIdx) {
                        if(matchedSupp[setBitIdx]){
                            errorVector.set(setBitIdx + checkMatrix.ColumnsSize() - checkMatrix.RowsSize());
                        }
                    }
                    // std::cout << checkMatrix.matVecMul(errorVector) << '\n';
                    // std::cout << syndrome << "\n\n";
                    // assert(checkMatrix.matVecMul(errorVector) == syndrome);

                    return std::optional<boost::dynamic_bitset<>>(errorVector);
                }
            }
        }

        return std::optional<boost::dynamic_bitset<>>();
    }
};
