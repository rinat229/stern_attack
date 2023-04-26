#pragma once

#include <boost/dynamic_bitset.hpp>

#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>
#include <optional>

#include <algebra/binary_matrix.hpp>
#include <permutations/combination_iterator.hpp>
#include "base_decoding.hpp"


class InformationSetDecoding : public BaseAlgorithm {
    int p;

public:
    constexpr static const char* algorithmName = "InformationSetDecoding";

    /**
     * Creates an options object for ISD algorithm
     * @param cols number of columns in matrix, %p will initialize as 0.003 * cols (it's optimal parameter)
    */
    InformationSetDecoding(unsigned cols) : p(static_cast<unsigned>(0.003 * cols) > 0 ? 0.003 * cols : 1) {}

    unsigned getParams() const {
        return p;
    }

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
        unsigned colsSizeOfQ = cols - rows;

        for(auto combinationIter = Combination(p, colsSizeOfQ).begin(); combinationIter.CombinationsStillExist(); ++combinationIter) {
            if((checkMatrix.sumOfColumns(*combinationIter) ^ syndrome).count() == omega - p){
                boost::dynamic_bitset<> errorVector(cols);

                for(unsigned setBitIdx = 0; setBitIdx < p; ++setBitIdx) {
                    errorVector.set((*combinationIter)[setBitIdx]);
                }

                const auto matchedSupp = checkMatrix.sumOfColumns(*combinationIter) ^ syndrome;
                for(boost::dynamic_bitset<>::size_type setBitIdx = 0; setBitIdx < matchedSupp.size(); ++setBitIdx) {
                    if(matchedSupp[setBitIdx]){
                        errorVector.set(setBitIdx + colsSizeOfQ);
                    }
                }

                return std::optional<boost::dynamic_bitset<>>(errorVector);
            }
        }

        return std::optional<boost::dynamic_bitset<>>();
    }
};
