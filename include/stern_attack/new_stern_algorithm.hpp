#pragma once

#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>
#include <unordered_map>

#include <algebra/binary_matrix.hpp>
#include <permutations/combination_iterator.hpp>
#include <permutations/primitive_permutation_iterator.hpp>
#include <permutations/random_permutation_iterator.hpp>
#include <utils/cartesian_product.hpp>
#include "base_decoding.hpp"


class NewSternAlgorithm : public BaseAlgorithm {
    int p;
    int l;

public:
    constexpr static const char* algorithmName = "NewStern";

    /**
     * Creates an object for Stern algorithm
     * @param cols number of columns in matrix, p will initialize as 0.003 * cols
     * l as 0.013 * cols  (it's optimal parameters)
    */
    NewSternAlgorithm(unsigned cols) : p(static_cast<unsigned>(0.002 * cols) > 0 ? 0.002 * cols : 1),
                                l(static_cast<unsigned>(0.013 * cols) > 0 ? 0.013 * cols : 1) {}

    std::pair<unsigned, unsigned> getParams() const {
        return std::make_pair(p, l);
    }

    /**
     * Makes one step of Stern algorithm to already permuted check matrix
     * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
     * @param syndrome syndrome vector 
     * @param omega number of errors in codeword (i.e. number of ones in error vector)
     * @return filled std::optional<boost::dynamic_bitset<>> if success, else empty optional
     */
    std::optional<boost::dynamic_bitset<>> operator()(BinaryMatrix& checkMatrix, boost::dynamic_bitset<> syndrome, const unsigned omega) const {
        unsigned rows = checkMatrix.RowsSize();
        unsigned cols = checkMatrix.ColumnsSize();
        unsigned colsSizeOfQ = cols - rows;
        unsigned halfColsSizeOfQ = colsSizeOfQ / 2;

        using CollisionType = std::pair<boost::dynamic_bitset<>, std::vector<unsigned>>;

        std::vector<CollisionType> projectedSum2;
        boost::dynamic_bitset<> projectedSyndrome = Projection(syndrome, l);
        std::unordered_map<boost::dynamic_bitset<>, std::vector<std::vector<unsigned>>> projectSum1_columns;

        for(auto combinationIter = Combination(p, halfColsSizeOfQ).begin(); combinationIter.CombinationsStillExist(); ++combinationIter) {
            projectSum1_columns[checkMatrix.sumOfColumns(*combinationIter, l)].push_back(*combinationIter);

            std::vector<unsigned> shiftedCombination = *combinationIter;
            std::for_each(shiftedCombination.begin(), shiftedCombination.end(), [&halfColsSizeOfQ](auto& element){
                element += halfColsSizeOfQ;
            });

            projectedSum2.emplace_back(checkMatrix.sumOfColumns(shiftedCombination, l) ^ projectedSyndrome, shiftedCombination);
        }


        for(auto iter2 = projectedSum2.begin(); iter2 != projectedSum2.end(); iter2++) {
            auto find_syndrome = projectSum1_columns.find(iter2->first);
            if (find_syndrome != projectSum1_columns.end()) {
                std::vector<std::vector<unsigned>> vec_columns = find_syndrome->second;
                for (auto columns : vec_columns) {
                    if((checkMatrix.sumOfColumns(columns) ^ checkMatrix.sumOfColumns(iter2->second) ^ syndrome).count() == omega - 2 * p) {
                        boost::dynamic_bitset<> errorVector(cols);

                        for(unsigned setBitIdx = 0; setBitIdx < p; ++setBitIdx) {
                            errorVector.set(columns[setBitIdx]);
                            errorVector.set(iter2->second[setBitIdx]);
                        }

                        const auto matchedSupp = checkMatrix.sumOfColumns(columns) ^ checkMatrix.sumOfColumns(iter2->second) ^ syndrome;
                        for(boost::dynamic_bitset<>::size_type setBitIdx = 0; setBitIdx < matchedSupp.size(); ++setBitIdx) {
                            if(matchedSupp[setBitIdx]){
                                errorVector.set(setBitIdx + colsSizeOfQ);
                            }
                        }

                        return std::optional<boost::dynamic_bitset<>>(errorVector);
                    }
                }
            }
        }



        return std::optional<boost::dynamic_bitset<>>();
    }
};