#pragma once

#include <boost/dynamic_bitset.hpp>
#include <random>
#include <algorithm>
#include <cassert>
#include <functional>

#include <algebra/binary_matrix.hpp>


/**
 * Algorithm for projecting first l elements of bitset
 * @param bitset_ bitset
 * @param l number of elements
 * @return projected bitset
*/
boost::dynamic_bitset<> Projection(const boost::dynamic_bitset<>& bitset_, unsigned l) {
    boost::dynamic_bitset<> projectedBitset = bitset_;
    projectedBitset.resize(l);
    // for(boost::dynamic_bitset<>::size_type idx = 0; idx < l; ++idx) {
    //     projectedBitset[idx] = bitset_[idx];
    // }

    return projectedBitset;
}


/**
 * Base algorithm of decoding that find an error vector e
 * for check matrix H and syndrome s such that H*e = s
 * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
 * @param syndrome syndrome vector 
 * @param omega number of errors in codeword (i.e. number of ones in error vector)
 * @param algorithm algorithm for decoding (for example, ISD, Stern)
 * @return error vector
 */
template <typename DecodingStepAlgorithm>
boost::dynamic_bitset<> Decoding(BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome,
                                 const unsigned omega, const DecodingStepAlgorithm algorithm){
    unsigned cols = checkMatrix.ColumnsSize();

    boost::dynamic_bitset<> errorVector(cols);

    for(auto permutationIter = RandomPermutation(cols).begin(); permutationIter.CanBePermuted(); ++permutationIter) {
        
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

    return errorVector;
}
