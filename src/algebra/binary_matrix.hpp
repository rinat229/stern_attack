#pragma once

#include <vector>
#include <cassert>

#include <boost/dynamic_bitset.hpp>


class BinaryMatrix {
public:
    using DataType = std::vector<boost::dynamic_bitset<>>;

private:
    DataType matrix;


public:
    BinaryMatrix(const DataType& matrix): matrix(matrix) {}

    BinaryMatrix(const DataType&& matrix): matrix(std::move(matrix)) {}

    BinaryMatrix applyPermutation(const std::vector<int>& permutation) {
        // assert(permutation.size() == columnsSize());

        DataType permutedMatrix = getData();
        const auto rows = rowsSize();

        for(auto rowIndex = 0; rowIndex < rows; ++rowIndex) {
            for(auto colIndex = 0; colIndex < permutation.size(); ++colIndex){
                permutedMatrix[rowIndex][colIndex] = permutedMatrix[rowIndex][permutation[colIndex]];
            }
        }

        return BinaryMatrix(std::move(permutedMatrix));
    }

    unsigned int rowsSize() const {
        return matrix.size();
    }

    unsigned int columnsSize() const {
        return matrix.front().size();
    }

    DataType& getData() {
        return matrix;
    }
};
