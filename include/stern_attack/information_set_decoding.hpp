#pragma once

#include <boost/dynamic_bitset.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>

#include <algebra/binary_matrix.hpp>
#include <permutations/combination_iterator.hpp>
#include <permutations/primitive_permutation_iterator.hpp>
#include <permutations/random_permutation_iterator.hpp>

#include <omp.h>


/**
 * Makes one step of information-set decoding to already permuted check matrix
 * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
 * @param syndrome syndrome vector 
 * @param p number of columns that should be summed
 * @param omega number of errors in codeword (i.e. number of ones in error vector)
 * @return <true, matched error vector> if success, <false, empty bitset> else
 */
std::pair<bool, boost::dynamic_bitset<> > Step(BinaryMatrix& checkMatrix, boost::dynamic_bitset<> syndrome, const unsigned p, const unsigned omega) {
    if(!GaussElimination(checkMatrix, syndrome)){
        return std::make_pair(false, boost::dynamic_bitset<>());
    }

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

            // assert(checkMatrix.matVecMul(errorVector) == syndrome);

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
boost::dynamic_bitset<> InformationSetDecoding(BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome,const unsigned omega){
    unsigned cols = checkMatrix.ColumnsSize();
    unsigned p = static_cast<unsigned>(0.003 * cols) > 0 ? 0.003 * cols : 1;
    boost::dynamic_bitset<> errorVector(cols);

    for(auto permutationIter = RandomPermutation(cols).begin(); permutationIter.CanBePermuted(); ++permutationIter) {
        
        BinaryMatrix permutedCheckMatrix = checkMatrix.applyPermutation(*permutationIter);
        auto [isFinded, permutedErrorVector] = Step(permutedCheckMatrix, syndrome, p, omega);

        if(isFinded) {
            for(unsigned idx = 0; idx < permutationIter->size(); ++idx){
                errorVector[(*permutationIter)[idx]] = permutedErrorVector[idx];
            }
            assert(checkMatrix.matVecMul(errorVector) == syndrome);

            break;
        }
    }

    return errorVector;
}


void Thread_Task(RandomPermutation::RandomPermutationIterator& permIter, bool& IsFinded, BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome,const unsigned omega,
                 const unsigned p, boost::dynamic_bitset<>& errorVector, boost::mutex &mx ) {
    boost::dynamic_bitset<> permutedErrorVector;
    mx.lock();
    while  ((!IsFinded) && (permIter.CanBePermuted())) {
        auto my_iter = permIter;
        permIter++;
        mx.unlock();
        BinaryMatrix permutedCheckMatrix = checkMatrix.applyPermutation(*my_iter);
        std::pair<bool, boost::dynamic_bitset<> > ans = Step(permutedCheckMatrix, syndrome, p, omega);
        IsFinded = ans.first;
        permutedErrorVector = ans.second;

        mx.lock();
        if(ans.first) {
            for(unsigned idx = 0; idx < my_iter->size(); ++idx){
                errorVector[(*my_iter)[idx]] = permutedErrorVector[idx];
            }
            assert(checkMatrix.matVecMul(errorVector) == syndrome);
        }
    }
    mx.unlock();
    return;

}


/**
 * Parallel algorithm of information-set decoding that find an error vector e, It works faster
 * for check matrix H and syndrome s such that H*e = s
 * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
 * @param syndrome syndrome vector 
 * @param omega number of errors in codeword (i.e. number of ones in error vector)
 * @return error vector
 */
boost::dynamic_bitset<> InformationSetDecodingParallel(BinaryMatrix& checkMatrix, boost::dynamic_bitset<>& syndrome,const unsigned omega, const int &num_threads){
    unsigned cols = checkMatrix.ColumnsSize();
    unsigned p = static_cast<unsigned>(0.003 * cols) > 0 ? 0.003 * cols : 1;
    boost::dynamic_bitset<> errorVector(cols);

    auto permutationIter = RandomPermutation(cols).begin();
    bool IsFinded = false;
    boost::mutex mx;
    boost::thread_group thr_group;
    for (int i = 0; i < num_threads; i++) {
        thr_group.create_thread(boost::bind(Thread_Task, boost::ref(permutationIter), boost::ref(IsFinded), boost::ref(checkMatrix),
                                            boost::ref(syndrome), omega, p, boost::ref(errorVector), boost::ref(mx)));
    }

    thr_group.join_all();


    return errorVector;
}