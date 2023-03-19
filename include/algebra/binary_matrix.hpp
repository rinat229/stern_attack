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
        os << row << '\n';
    }

    return os;
}

void SwapRows(BinaryMatrix &Matrix,const int &i,const int &k) {
    std::swap(Matrix[i], Matrix[k]);
}

void SwapBits(boost::dynamic_bitset<> &bits, const int &i, const int &k) {
    // i and k position from least bit
    bool temp = (bool)bits[i];
    bits[i] = bits[k];
    bits[k] = temp; 
}

bool GaussElimination(BinaryMatrix &Matrix, boost::dynamic_bitset<> &syndrom) {
    int rows = Matrix.rowsSize(); // rows = n - k
    int columns = rows; // We need to change only right side from 0 to n - k column


    // Transform to upper triangle from for right side
    int i = 0;
    for (int j = columns - 1; j >= 0; j--) {
        std::vector<int> pos; // Find ALl pos where 1 
        for (int z = i + 1; z < rows; z++) {
            if ((bool)Matrix[z][j]) {
                pos.push_back(z);                
            }
        }


        // Find zero in column
        if (!((bool)Matrix[i][j])) {

            // We need to check, if there are a non zero element in column
            if (pos.empty()) { // Go to Next Column
                i++;
                continue; 
            } else { // Swipe Rows
                SwapRows(Matrix, i, pos.back());
                SwapBits(syndrom, rows - 1 - i, rows - 1 - pos.back());
                pos.pop_back();
            } 
        }


        for (int k = 0; k < pos.size(); k++) {
            Matrix[pos[k]] = Matrix[i] ^ Matrix[pos[k]];
            syndrom[rows - 1 - pos[k]] = syndrom[rows - 1 - i] ^ syndrom[rows - 1 - pos[k]];
        }
        i++;
    }
    
    bool det = 1;
    for (i = 0; i < rows; i++) {
        det = ((bool)Matrix[i][columns - 1 - i] && det);
    }

    if (!det) { // if det == 0
        return false;
    }

    i = rows - 1; // last row
    for (int j = 0; j < columns; j++) {
        for (int z = i - 1; z >= 0; z--) {
            if ((bool)Matrix[z][j]) {
                Matrix[z] = Matrix[z] ^ Matrix[i];
                syndrom[rows - 1 - z] = syndrom[rows - 1 -z] ^ syndrom[rows - 1 - i];
            }
        }
        i--;
    }

    return true;
}

