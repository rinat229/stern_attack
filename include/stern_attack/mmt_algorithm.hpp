#pragma once

#include <boost/dynamic_bitset.hpp>

#include <iostream>
#include <algorithm>
#include <cassert>
#include <execution>


#include <algebra/binary_matrix.hpp>
#include <permutations/combination_iterator.hpp>
#include <permutations/primitive_permutation_iterator.hpp>
#include <permutations/random_permutation_iterator.hpp>
#include "base_decoding.hpp"


class MMTAlgorithm : public BaseAlgorithm {

    static constexpr unsigned maxSizeOfProjSumOnLevel2 = 1000000;
    unsigned p;
    unsigned l1;
    unsigned l2;
    unsigned l;

public:
    constexpr static const char* algorithmName = "MMT";

    /**
     * Creates an options object for MMT algorithm
     * @param cols number of columns in matrix, p initializes as 0.006 * cols (it's optimal parameter)
     * l1 as 0.028 * cols, l2 as 0.006 * cols
    */
    MMTAlgorithm(unsigned cols) : p(static_cast<unsigned>(0.002 * cols) > 0 ? 0.002 * cols : 1),
                                            l1(static_cast<unsigned>(0.028 * cols) > 0 ? 0.028 * cols : 1),
                                            l2(static_cast<unsigned>(0.006 * cols) > 0 ? 0.006 * cols : 1),
                                            l(l1 + l2) {}


    /**
     * Makes one step of information-set decoding to already permuted check matrix
     * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
     * @param syndrome syndrome vector 
     * @param omega number of errors in codeword (i.e. number of ones in error vector)
     * @return filled std::optional<boost::dynamic_bitset<>> if success, else empty optional
     */
    std::optional<boost::dynamic_bitset<>> operator()(BinaryMatrix& checkMatrix, boost::dynamic_bitset<> syndrome, const unsigned omega) const {
        unsigned rows = checkMatrix.RowsSize();
        unsigned cols = checkMatrix.ColumnsSize();
        unsigned colsSizeOfQ = cols - rows + l;

        unsigned halfColsSizeOfQ = colsSizeOfQ / 2;

        using CollisionType = std::pair<BinaryMatrix::BitContainerType, std::vector<unsigned>>;
        
        auto combination = Combination(p, halfColsSizeOfQ);
        auto numberOfCombs = combination.GetNumberOfCombinations();

        std::vector<CollisionType> projectedSum1, projectedSum2;
        std::vector<CollisionType> projectedSum11, projectedSum12;
        std::vector<CollisionType> projectedSum21, projectedSum22;

        projectedSum11.reserve(numberOfCombs);
        projectedSum12.reserve(numberOfCombs);
        projectedSum21.reserve(numberOfCombs);
        projectedSum22.reserve(numberOfCombs);

        projectedSum1.reserve(maxSizeOfProjSumOnLevel2);
        projectedSum2.reserve(maxSizeOfProjSumOnLevel2);

        boost::dynamic_bitset<> projectedSyndrome1 = Projection(syndrome, l1);
        boost::dynamic_bitset<> projectedSyndrome2 = Projection(syndrome, l, l1);

        BinaryMatrix checkMatrixTr = checkMatrix.TransposeMatrix(0, checkMatrix.RowsSize());
        BinaryMatrix checkMatrixOnL1 = checkMatrix.TransposeMatrix(0, l1);
        BinaryMatrix checkMatrixOnL2 = checkMatrix.TransposeMatrix(l1, l);

        for(auto combinationIter = combination.begin(); combinationIter.CombinationsStillExist(); ++combinationIter) {
            /// TODO: projectedSum11 == projectedSum21 ???
            projectedSum11.emplace_back(checkMatrixOnL2.sumOfRows(*combinationIter), *combinationIter);
            projectedSum21.emplace_back(checkMatrixOnL2.sumOfRows(*combinationIter), *combinationIter);

            std::vector<unsigned> shiftedCombination = *combinationIter;
            std::for_each(shiftedCombination.begin(), shiftedCombination.end(), [&halfColsSizeOfQ](auto& element){
                element += halfColsSizeOfQ;
            });

            projectedSum12.emplace_back(checkMatrixOnL2.sumOfRows(shiftedCombination), shiftedCombination);
            projectedSum22.emplace_back(checkMatrixOnL2.sumOfRows(shiftedCombination) ^ projectedSyndrome2, shiftedCombination);
        }

        auto compare = [](auto &a, auto& b) {
            return a.first < b.first;
        };

        std::sort(std::execution::unseq, projectedSum11.begin(), projectedSum11.end(), compare);
        std::sort(std::execution::unseq, projectedSum12.begin(), projectedSum12.end(), compare);
        std::sort(std::execution::unseq, projectedSum21.begin(), projectedSum21.end(), compare);
        std::sort(std::execution::unseq, projectedSum22.begin(), projectedSum22.end(), compare);

        for(auto iter1 = projectedSum11.begin(), iter2 = projectedSum12.begin(); iter1 != projectedSum11.end() && iter2 != projectedSum12.end();) {
            if(iter1->first < iter2->first){
                ++iter1;
            } else if(iter1->first > iter2->first) {
                ++iter2;
            } else {
                auto iterEnd1 = std::find_if(iter1, projectedSum11.end(), [&iter1] (auto &iter) {
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

                if(projectedSum1.size() > maxSizeOfProjSumOnLevel2) {
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
                            projectedSum2.emplace_back(checkMatrixOnL1.sumOfRows(indexUnion) ^ projectedSyndrome1, indexUnion);
                        }
                    }
                }

                if(projectedSum2.size() > maxSizeOfProjSumOnLevel2) {
                    break;
                }

                iter1 = iterEnd1;
                iter2 = iterEnd2;
            }
        }

        std::sort(projectedSum1.begin(), projectedSum1.end(), compare);
        std::sort(projectedSum2.begin(), projectedSum2.end(), compare);

        // indexes of columns should guarantee to be sorted for finding symmetric difference
        std::for_each(projectedSum1.begin(), projectedSum1.end(), [](CollisionType& element) {
            std::sort(element.second.begin(), element.second.end());
        });

        std::for_each(projectedSum2.begin(), projectedSum2.end(), [](CollisionType& element) {
            std::sort(element.second.begin(), element.second.end());
        });

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

                for(auto iterMatched1 = iter1; iterMatched1 != iterEnd1; ++iterMatched1){
                    for(auto iterMatched2 = iter2; iterMatched2 != iterEnd2; ++iterMatched2){
                        std::vector<unsigned> indexColumns;
                        indexColumns.reserve(iterMatched1->second.size() + iterMatched2->second.size());

                        std::set_symmetric_difference(iterMatched1->second.begin(), iterMatched1->second.end(),
                                                    iterMatched2->second.begin(), iterMatched2->second.end(),
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

                iter1 = iterEnd1;
                iter2 = iterEnd2;
            }
        }

        return std::optional<boost::dynamic_bitset<>>();
    }
};
