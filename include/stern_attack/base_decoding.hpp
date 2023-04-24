#pragma once

#include <boost/dynamic_bitset.hpp>
#include <random>
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <omp.h>

#include <algebra/binary_matrix.hpp>
#include <permutations/random_permutation_iterator.hpp>


/**
 * Algorithm for projecting first end elements of bitset
 * @param bitset bitset
 * @param end index of the last element to be copied
 * @param start index of the first element to be copied (0 by default)
 * @return projected bitset
*/
boost::dynamic_bitset<> Projection(const boost::dynamic_bitset<>& bitset, unsigned end, unsigned start = 0) {
    boost::dynamic_bitset<> projectedBitset;

    if(start == 0){
        projectedBitset = bitset;
        projectedBitset.resize(end);
    } else {
        projectedBitset.resize(end - start);

        for(boost::dynamic_bitset<>::size_type idx = start; idx < end; ++idx) {
            projectedBitset[idx] = bitset[idx];
        }
    }

    return projectedBitset;
}


/**
 * Base algorithm of decoding that find an error vector e
 * for check matrix H and syndrome s such that H*e = s
 * @tparam DecodingStepAlgorithm type of algorithm (for example, ISD, Stern)
 * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
 * @param syndrome syndrome vector 
 * @param omega number of errors in codeword (i.e. number of ones in error vector)
 * @param algorithm algorithm for decoding 
 * @return error vector
 */
template <typename DecodingStepAlgorithm>
boost::dynamic_bitset<> Decoding(BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome,
                                 const unsigned omega, const DecodingStepAlgorithm algorithm){
    unsigned cols = checkMatrix.ColumnsSize();

    boost::dynamic_bitset<> errorVector(cols);
    unsigned numberOfIterations = 0;
    for(auto permutationIter = RandomPermutation(cols).begin(); permutationIter.CanBePermuted(); ++permutationIter, ++numberOfIterations) {
        
        BinaryMatrix permutedCheckMatrix = checkMatrix.applyPermutation(*permutationIter);
        auto permutedErrorVector = algorithm(permutedCheckMatrix, syndrome, omega);

        if(permutedErrorVector) {
            for(unsigned idx = 0; idx < permutationIter->size(); ++idx){
                errorVector[(*permutationIter)[idx]] = (*permutedErrorVector)[idx];
            }
            // assert(checkMatrix.matVecMul(errorVector) == syndrome);

            break;
        }
    }

    std::cout << "Algorithm: " << algorithm.algorithmName << ",\tsize: " << checkMatrix.ColumnsSize() << ",\tnumber of iterations: " << numberOfIterations << '\n';
    return errorVector;
}
