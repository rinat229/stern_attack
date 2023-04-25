#pragma once

#include <boost/dynamic_bitset.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

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

// void ThreadTask(RandomPermutation::RandomPermutationIterator& permIter, bool& IsFinded, BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome,const unsigned omega,
//                  const unsigned p, boost::dynamic_bitset<>& errorVector, boost::mutex &mx ) {
//     boost::dynamic_bitset<> permutedErrorVector;
//     mx.lock();
//     while  ((!IsFinded) && (permIter.CanBePermuted())) {
//         auto my_iter = permIter;
//         permIter++;
//         mx.unlock();
//         BinaryMatrix permutedCheckMatrix = checkMatrix.applyPermutation(*my_iter);
//         auto permutedErrorVector = InformationSetDecodingStep(permutedCheckMatrix, syndrome, p, omega);

//         mx.lock();
//         if(permutedErrorVector) {
//             for(unsigned idx = 0; idx < my_iter->size(); ++idx){
//                 errorVector[(*my_iter)[idx]] = (*permutedErrorVector)[idx];
//             }
//             assert(checkMatrix.matVecMul(errorVector) == syndrome);
//         }
//     }
//     mx.unlock();
//     return;

// }


// /**
//  * Parallel algorithm of information-set decoding that find an error vector e, It works faster
//  * for check matrix H and syndrome s such that H*e = s
//  * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
//  * @param syndrome syndrome vector 
//  * @param omega number of errors in codeword (i.e. number of ones in error vector)
//  * @param numThreads number of threads that can be used in algorithm
//  * @return error vector
//  */
// boost::dynamic_bitset<> InformationSetDecodingParallel(BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome,const unsigned omega, const int &numThreads){
//     unsigned cols = checkMatrix.ColumnsSize();
//     unsigned p = static_cast<unsigned>(0.003 * cols) > 0 ? 0.003 * cols : 1;
//     boost::dynamic_bitset<> errorVector(cols);

//     auto permutationIter = RandomPermutation(cols).begin();
//     bool IsFinded = false;
//     boost::mutex mx;
//     boost::thread_group thr_group;
//     for (int i = 0; i < numThreads; i++) {
//         thr_group.create_thread(boost::bind(ThreadTask, boost::ref(permutationIter), boost::ref(IsFinded), boost::ref(checkMatrix),
//                                             boost::ref(syndrome), omega, p, boost::ref(errorVector), boost::ref(mx)));
//     }

//     thr_group.join_all();


//     return errorVector;
// }