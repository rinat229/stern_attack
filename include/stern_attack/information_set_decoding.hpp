#pragma once

#include <boost/dynamic_bitset.hpp>

#include <algebra/binary_matrix.hpp>
#include <permutations/combination_iterator.hpp>
#include <permutations/primitive_permutation_iterator.hpp>


/**
 * Makes one step of information-set decoding to already permuted check matrix
 * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
 * @param syndrome syndrome vector 
 * @param p number of columns that should be summed
 * @param omega number of errors in codeword (i.e. number of ones in error vector)
 * @return <true, matched error vector> if success, <false, empty bitset> else
 */
std::pair<bool, boost::dynamic_bitset<> > Step(BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome, unsigned p, unsigned omega) {
    // ...
    // applying row elimination to check matrix and syndrome
    // ...

    unsigned rows = checkMatrix.rowsSize();
    unsigned cols = checkMatrix.columnsSize();
    unsigned colsSizeOfQ = cols - rows;

    for(auto combinationIter = Combination(p, colsSizeOfQ).begin(); combinationIter.CombinationsStillExist(); ++combinationIter) {
        if((checkMatrix.sumOfColumns(*combinationIter) ^ syndrome).count() == p){
            boost::dynamic_bitset<> errorVector(cols);

            for(unsigned setBitIdx = 0; setBitIdx < p; ++setBitIdx) {
                errorVector.set((*combinationIter)[setBitIdx]);
            }

            unsigned idxOfOnesFromIdentityMatrix = colsSizeOfQ + omega - p;
            for(unsigned setBitIdx = colsSizeOfQ; setBitIdx < idxOfOnesFromIdentityMatrix; ++setBitIdx) {
                errorVector.set((*combinationIter)[setBitIdx]);
            }

            return std::make_pair(true, errorVector);
        }
    }

    return std::make_pair(false, boost::dynamic_bitset<>());
}


/**
 * Sequential algorithm of information-set decoding that find an error vector e
 * for check matrix H and syndrome s such that H*e = s
 * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
 * @param syndrome syndrome vector 
 * @param omega number of errors in codeword (i.e. number of ones in error vector)
 * @return error vector
 */
boost::dynamic_bitset<> InformationSetDecoding(BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome, unsigned omega){
    unsigned cols = checkMatrix.columnsSize();
    unsigned p = 0.003 * cols;

    for(auto permutationIter = PrimitivePermutation(cols).begin(); permutationIter.CanBePermuted(); ++permutationIter) {
        BinaryMatrix permutedCheckMatrix = checkMatrix.applyPermutation(*permutationIter);
        auto [isFinded, errorVector] = Step(permutedCheckMatrix, syndrome, p, omega);

        if(isFinded) {
            return errorVector;
        }
    }

    return boost::dynamic_bitset<>();
}
