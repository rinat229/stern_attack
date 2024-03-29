#pragma once

#include <vector>
#include <cassert>
#include <optional>
#include <omp.h>

#include <boost/dynamic_bitset.hpp>


class BinaryMatrix {
public:
    using BitContainerType = boost::dynamic_bitset<>;
    using DataType = std::vector<BitContainerType>;
    using size_type = std::size_t;

private:
    DataType matrix;


public:
    BinaryMatrix() = default;

    BinaryMatrix(const DataType& data): matrix(data) {}

    BinaryMatrix(const DataType&& data): matrix(std::move(data)) {}

    BinaryMatrix(const std::initializer_list<std::string>& data) {
        matrix.reserve(data.size());

        for(const auto& row: data){
            matrix.emplace_back(row);
        }
    }

    BinaryMatrix applyPermutation(const std::vector<unsigned>& permutation) {
        // assert(permutation.size() == ColumnsSize());

        DataType permutedMatrix = getData();
        const auto rows = RowsSize();

        for(auto rowIndex = 0; rowIndex < rows; ++rowIndex) {
            for(auto colIndex = 0; colIndex < permutation.size(); ++colIndex){
                permutedMatrix[rowIndex][colIndex] = matrix[rowIndex][permutation[colIndex]];
            }
        }

        return BinaryMatrix(std::move(permutedMatrix));
    }

    unsigned int RowsSize() const {
        return matrix.size();
    }

    unsigned int ColumnsSize() const {
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

    /// TODO: optimize this 
    BitContainerType sumOfColumns(const std::vector<unsigned>& indexes, const unsigned endRow = 0) const{
        unsigned rows = endRow == 0 ? RowsSize() : endRow;

        return sumOfColumns(indexes, 0, rows);
    }

    BitContainerType sumOfColumns(const std::vector<unsigned>& indexes, const unsigned startRow, const unsigned endRow) const {
        BitContainerType resultSum(endRow - startRow);

        for(unsigned idxRow = startRow, idxSumRow = 0; idxRow < endRow; ++idxRow, ++idxSumRow) {
            bool resultForRow = 0;

            for(auto& idxCol: indexes){
                resultForRow ^= matrix[idxRow][idxCol];
            }

            resultSum[idxSumRow] = resultForRow;
        }

        return resultSum;
    }

    BitContainerType sumOfRows(const std::vector<unsigned>& indexes) const {
        BitContainerType resultSum(ColumnsSize());

        for(auto &idx: indexes) {
            resultSum ^= matrix[idx];
        }

        return resultSum;
    }

    void addRow(const BitContainerType& row) {
        matrix.push_back(row);
    }

    void addRow(const std::string& row){
        addRow(std::move(BitContainerType(row)));
    }

    BitContainerType matVecMul(const BitContainerType& x) const {
        assert(x.size() == ColumnsSize());

        BitContainerType result(RowsSize());

        for(BitContainerType::size_type row = 0; row < RowsSize(); ++row) {
            result.set(row, (matrix[row] & x).count() % 2);
        }

        return result;
    }

    BinaryMatrix TransposeMatrix(unsigned idxFirstRow, unsigned idxLastRow) {
        BinaryMatrix transposedMatrix;
        unsigned cols = ColumnsSize();
        unsigned rowsOfTrMatrix = idxLastRow - idxFirstRow;

        for(unsigned colIdx = 0; colIdx < cols; ++colIdx) {
            BinaryMatrix::BitContainerType column(rowsOfTrMatrix);

            for(unsigned idxTr = 0, rowIdx = idxFirstRow; rowIdx < idxLastRow; ++rowIdx, ++idxTr) {
                column[idxTr] = matrix[rowIdx][colIdx];
            }

            transposedMatrix.addRow(std::move(column));
        }

        return transposedMatrix;
    }

    static bool GaussElimination(BinaryMatrix &matrix, boost::dynamic_bitset<> &syndrome);
};

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, BinaryMatrix& b){
    for(const auto& row: b.getData()){
        for(BinaryMatrix::BitContainerType::size_type i = 0; i < row.size(); ++i){
            os << row[i];
        }

        os << '\n';
    }

    return os;
}


bool BinaryMatrix::GaussElimination(BinaryMatrix &matrix, boost::dynamic_bitset<> &syndrome) {
    int rows = matrix.RowsSize(); // rows = n - k
    int columns = matrix.ColumnsSize(); // columns = n
    int k = columns - rows;


    // Transform to upper triangle from for right side
    int i = 0;
    for (int j = k; j < columns; j++) {
        std::vector<int> pos; // Find ALl pos where 1 
        for (int z = i + 1; z < rows; z++) {
            if ((bool)matrix[z][j]) {
                pos.push_back(z);                
            }
        }

        // Find zero in column
        if (!((bool)matrix[i][j])) {

            // We need to check, if there are a non zero element in column
            if (pos.empty()) { // Go to Next Column
                i++;
                continue; 
            } else { // Swipe Rows
                std::swap(matrix[i], matrix[pos.back()]);

                bool temp = (bool)syndrome[i];
                syndrome[i] = syndrome[pos.back()];
                syndrome[pos.back()] = temp; 
                pos.pop_back();
            } 
        }

        for (int k = 0; k < pos.size(); k++) {
            matrix[pos[k]] ^= matrix[i];
            syndrome[pos[k]] ^= syndrome[i];
        }
        i++;
    }
    
    bool det = 1;
    for (i = 0; i < rows; i++) {
        det = ((bool)matrix[i][k + i] && det);
    }

    if (!det) { // if det == 0
        return false;
    }

    i = rows - 1; // last row
    for (int j = columns - 1; j >= k; j--) {
        for (int z = i - 1; z >= 0; z--) {
            if ((bool)matrix[z][j]) {
                matrix[z] ^= matrix[i];
                syndrome[z] ^= syndrome[i];
            }
        }
        i--;
    }

    return true;
}

