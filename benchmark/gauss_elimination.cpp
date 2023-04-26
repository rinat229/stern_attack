#include <algebra/binary_matrix.hpp>
#include <utils/benchmark.hpp>

#include <vector>
#include <iostream>
#include <random>

#include <boost/dynamic_bitset.hpp>


using MatrixWithSyndromeType = std::pair<BinaryMatrix, boost::dynamic_bitset<>>;

std::vector<MatrixWithSyndromeType> GenerateRandomMatrices(unsigned numberOfMatrices, unsigned rows, unsigned cols){
    std::vector<MatrixWithSyndromeType> matrices;
    matrices.reserve(numberOfMatrices);

    std::random_device rd;
    std::mt19937 gen( rd());
    std::bernoulli_distribution d(0.5);

    for(int i = 0; i < numberOfMatrices; ++i) {
        BinaryMatrix matrix;
        boost::dynamic_bitset<> syndrome(rows);

        for(int j = 0; j < rows; ++j) {
            BinaryMatrix::BitContainerType row(cols);
            for(int k = 0; k < cols; ++k) {
                row[k] = d(gen);
            }
            matrix.addRow(row);
            syndrome[j] = d(gen);
        }

        matrices.emplace_back(matrix, syndrome);
    }

    return matrices;
}

int main() {
    const int numberOfMatrices = 300, rows = 1000, cols = 1000;

    std::vector<MatrixWithSyndromeType> randomMatrices = GenerateRandomMatrices(numberOfMatrices, rows, cols);

    {
        Timer timer(std::string("Gauss elimination"), numberOfMatrices);
        for(auto &[checkMatrix, syndrome]: randomMatrices) {
            BinaryMatrix::GaussElimination(checkMatrix, syndrome);
        }
    }
}