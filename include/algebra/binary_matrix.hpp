#pragma once

#include <vector>
#include <cassert>

#include <boost/dynamic_bitset.hpp>


class BinaryMatrix {
public:
    using BitContainerType = boost::dynamic_bitset<>;
    using DataType = std::vector<BitContainerType>;
    using size_type = std::size_t;

private:
    DataType matrix;


public:
    BinaryMatrix(const DataType& data): matrix(data) {}

    BinaryMatrix(const DataType&& data): matrix(std::move(data)) {}

    BinaryMatrix(const std::initializer_list<std::string>& data) {
        matrix.reserve(data.size());

        for(const auto& row: data){
            matrix.emplace_back(row);
        }
    }

    BinaryMatrix applyPermutation(const std::vector<int>& permutation) {
        // assert(permutation.size() == columnsSize());

        DataType permutedMatrix = getData();
        const auto rows = rowsSize();

        for(auto rowIndex = 0; rowIndex < rows; ++rowIndex) {
            for(auto colIndex = 0; colIndex < permutation.size(); ++colIndex){
                permutedMatrix[rowIndex][colIndex] = matrix[rowIndex][permutation[colIndex]];
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

    BitContainerType& operator[](size_type pos){
        return matrix[pos];
    }

    bool operator==(const BinaryMatrix& rhs){
        return rhs.matrix == matrix;
    }
};

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, BinaryMatrix& b){
    for(const auto& row: b.getData()){
        for(BinaryMatrix::BitContainerType::size_type i = 0; i < row.size(); ++i){
            std::cout << row[i];
        }

        std::cout << '\n';
    }

    return os;
}

void SwapRows(BinaryMatrix &Matrix,const int &i,const int &k) {
    std::swap(Matrix[i], Matrix[k]);
}

void GausElimination(BinaryMatrix &Matrix) {
    int rows = Matrix.rowsSize(); // rows = n - k
    int columns = rows; // We need to change only right side from 0 to n - k column

    // Transform to upper triangle from for right side
    for (int i = 0; i < rows; i++) {
        for (int j = rows - 1; j >= 0; j--) {
            // Find not zero in column
            if (!((bool)Matrix[i][j])) {
                int index = -1;
                for (int z = i + 1; z < rows; z++) {
                    if ((bool)Matrix[z][j]) {
                        index = z;
                    }
                }

                // We need to check, if there are a non zero element in column
                if (index == -1) { // Go to Next Column
                    continue; 
                } else { // Swipe Rows
                    SwapRows(Matrix, i, index);
                }
            }
        }
    }

    return;
}
