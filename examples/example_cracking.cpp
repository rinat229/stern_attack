#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <iostream>
#include <boost/dynamic_bitset.hpp>

#include <algebra/binary_matrix.hpp>

#include <stern_attack/mmt_algorithm.hpp>
#include <stern_attack/stern_algorithm.hpp>
#include <stern_attack/information_set_decoding.hpp>

#include <utils/benchmark.hpp>


std::vector<std::string> ReadLinesFromFile(const std::string& file) {
    assert(std::filesystem::exists(file));

    std::ifstream ifs(file);
    std::vector<std::string> lines;
    std::string line;

    while(std::getline(ifs, line)) {
        lines.push_back(line);
    }

    return lines;
}

int main(int argc, char** argv) {
    if(argc != 3) {
        std::cerr << "provide a path to the data and number of errors\n";
        return -1;
    }

    std::string directory(argv[1]);
    directory += '/';
    unsigned omega = std::atoi(argv[2]);

    auto inputDataForMatrix = ReadLinesFromFile(directory + "check_matrix.txt");
    auto inputDataForSyndrome = ReadLinesFromFile(directory + "syndrome.txt");

    BinaryMatrix checkMatrix;
    for(auto& line: inputDataForMatrix) {
        std::reverse(line.begin(), line.end());
        checkMatrix.addRow(line);
    }

    std::reverse(inputDataForSyndrome.front().begin(), inputDataForSyndrome.front().end());
    boost::dynamic_bitset<> syndrome(inputDataForSyndrome.front());

    auto launchAndBenchmark = [&checkMatrix, &syndrome, &omega](auto algorithm){
        std::string name = std::string(algorithm.algorithmName) + std::to_string(checkMatrix.ColumnsSize());
        Timer timer(name);

        return Decoding(checkMatrix, syndrome, omega, algorithm);
    };

    auto launchAndBenchmarkParallel = [&checkMatrix, &syndrome, &omega](auto algorithm){
        std::string name = algorithm.algorithmName + std::string("(parallel ") + std::to_string(std::thread::hardware_concurrency()) + ")" + std::to_string(checkMatrix.ColumnsSize());
        Timer timer(name);

        return DecodingParallel(checkMatrix, syndrome, omega, algorithm, std::thread::hardware_concurrency());
    };

    boost::dynamic_bitset<> errorVectorFromMMT = launchAndBenchmark(MMTAlgorithm(checkMatrix.ColumnsSize()));
    std::ofstream ofs(directory + "error_vector.txt");
    ofs << errorVectorFromMMT << '\n';

    std::cout << "resulting error vector:\n" << errorVectorFromMMT << '\n';
}