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


bool PartialGaussElimination(BinaryMatrix &matrix, boost::dynamic_bitset<> &syndrome, int l) {
        int rows = matrix.RowsSize(); // rows = n - k
        int columns = matrix.ColumnsSize(); // columns = n
        int k = columns - rows;


        // Transform to upper triangle from for right side
        int i = l;
        for (int j = k + l; j < columns; j++) {
            std::vector<int> pos; // Find ALl pos where 1 
            for (int z = i + 1; z < rows; z++) {
                if ((bool)matrix[z][j]) {
                    pos.push_back(z);                
                }
            }

            // Find zero in column
            if (!((bool)matrix[i][j])) {

                // We need to check, if there are a non zero element in column
                if (pos.empty()) { // Go to Next Column
                    i++;
                    continue; 
                } else { // Swipe Rows
                    std::swap(matrix[i], matrix[pos.back()]);

                    bool temp = (bool)syndrome[i];
                    syndrome[i] = syndrome[pos.back()];
                    syndrome[pos.back()] = temp; 
                    pos.pop_back();
                } 
            }

            for (int k = 0; k < pos.size(); k++) {
                matrix[pos[k]] ^= matrix[i];
                syndrome[pos[k]] ^= syndrome[i];
            }
            i++;
        }
        
        bool det = 1;
        for (i = l; i < rows; i++) {
            det = ((bool)matrix[i][k + i] && det);
        }

        if (!det) { // if det == 0
            return false;
        }

        i = rows - 1; // last row
        for (int j = columns - 1; j >= (k + l); j--) {
            for (int z = i - 1; z >= 0; z--) {
                if ((bool)matrix[z][j]) {
                    matrix[z] ^= matrix[i];
                    syndrome[z] ^= syndrome[i];
                }
            }
            i--;
        }

        return true;
}


class FS_ISD_Algorithm : public BaseAlgorithm {
    int p;
    int l;

public:
    constexpr static const char* algorithmName = "FS-ISD";


    /**
     * Creates an object for Stern algorithm
     * @param cols number of columns in matrix, p will initialize as 0.003 * cols
     * l as 0.013 * cols  (it's optimal parameters)
    */
    FS_ISD_Algorithm(unsigned cols) : p(static_cast<unsigned>(0.002 * cols) > 0 ? 0.002 * cols : 1),
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
        
        if(!GaussElimination(checkMatrix, syndrome)) {
            return std::optional<boost::dynamic_bitset<>>();
        }
        

        unsigned rows = checkMatrix.RowsSize();
        unsigned cols = checkMatrix.ColumnsSize();
        unsigned colsSizeOfQ = cols - rows + l;
        unsigned halfColsSizeOfQ = colsSizeOfQ / 2;

        using CollisionType = std::pair<boost::dynamic_bitset<>, std::vector<unsigned>>;

        std::vector<CollisionType> projectedSum1, projectedSum2;
        boost::dynamic_bitset<> projectedSyndrome = Projection(syndrome, l);

        for(auto combinationIter = Combination(p, halfColsSizeOfQ).begin(); combinationIter.CombinationsStillExist(); ++combinationIter) {
            projectedSum1.emplace_back(checkMatrix.sumOfColumns(*combinationIter, l), *combinationIter);

            std::vector<unsigned> shiftedCombination = *combinationIter;
            std::for_each(shiftedCombination.begin(), shiftedCombination.end(), [&halfColsSizeOfQ](auto& element){
                element += halfColsSizeOfQ;
            });

            projectedSum2.emplace_back(checkMatrix.sumOfColumns(shiftedCombination, l) ^ projectedSyndrome, shiftedCombination);
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
                        for(boost::dynamic_bitset<>::size_type setBitIdx = l; setBitIdx < matchedSupp.size(); ++setBitIdx) {
                            if(matchedSupp[setBitIdx]){
                                errorVector.set(setBitIdx + colsSizeOfQ - l);
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
};