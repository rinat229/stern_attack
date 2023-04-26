#pragma once

#include <boost/dynamic_bitset.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <random>
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <omp.h>

#include <algebra/binary_matrix.hpp>
#include <permutations/random_permutation_iterator.hpp>


class BaseAlgorithm {
public:
    bool GaussElimination(BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome) const {
        return BinaryMatrix::GaussElimination(checkMatrix, syndrome);
    }
};


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

        #pragma omp simd
        for(boost::dynamic_bitset<>::size_type idx = start, idxProjected = 0; idx < end; ++idx, ++idxProjected) {
            projectedBitset[idxProjected] = bitset[idx];
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
        auto copiedSyndrome = syndrome;

        if(!algorithm.GaussElimination(permutedCheckMatrix, copiedSyndrome)){
            continue;
        }

        auto permutedErrorVector = algorithm(permutedCheckMatrix, copiedSyndrome, omega);

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


template <typename DecodingStepAlgorithm>
void ThreadTask(RandomPermutation::RandomPermutationIterator& permIter, BinaryMatrix& checkMatrix, 
                boost::dynamic_bitset<>& syndrome, boost::dynamic_bitset<>& errorVector, 
                unsigned omega, const DecodingStepAlgorithm& algorithm, boost::mutex &mx ) {
    std::optional<boost::dynamic_bitset<>> permutedErrorVector;
    mx.lock();
    while  ((!permutedErrorVector) && (permIter.CanBePermuted())) {
        auto my_iter = permIter;
        permIter++;
        mx.unlock();

        BinaryMatrix permutedCheckMatrix = checkMatrix.applyPermutation(*my_iter);
        auto copiedSyndrome = syndrome;

        if(!algorithm.GaussElimination(permutedCheckMatrix, copiedSyndrome)){
            continue;
        }

        auto permutedErrorVector = algorithm(permutedCheckMatrix, syndrome, omega);

        mx.lock();
        if(permutedErrorVector) {
            for(unsigned idx = 0; idx < my_iter->size(); ++idx){
                errorVector[(*my_iter)[idx]] = (*permutedErrorVector)[idx];
            }
        }
    }

    mx.unlock();
    return;
}


/**
 * Parallel algorithm of decoding that find an error vector e
 * for check matrix H and syndrome s such that H*e = s
 * @tparam DecodingStepAlgorithm type of algorithm (for example, ISD, Stern)
 * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
 * @param syndrome syndrome vector 
 * @param omega number of errors in codeword (i.e. number of ones in error vector)
 * @param algorithm algorithm for decoding 
 * @param numThreads number of threads 
 * @return error vector
 */
template <typename DecodingStepAlgorithm>
boost::dynamic_bitset<> DecodingParallel(BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome, 
                                         const unsigned omega, const DecodingStepAlgorithm& algorithm,
                                         const int &numThreads){
    unsigned cols = checkMatrix.ColumnsSize();
    boost::dynamic_bitset<> errorVector(cols);

    auto permutationIter = RandomPermutation(cols).begin();
    boost::mutex mx;
    boost::thread_group thr_group;
    for (int i = 0; i < numThreads; i++) {
        thr_group.create_thread(boost::bind(ThreadTask<DecodingStepAlgorithm>, boost::ref(permutationIter), boost::ref(checkMatrix),
                                boost::ref(syndrome), boost::ref(errorVector), omega,
                                boost::ref(algorithm), boost::ref(mx)));
    }

    thr_group.join_all();

    return errorVector;
}