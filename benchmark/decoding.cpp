#include <boost/dynamic_bitset.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include <algebra/binary_matrix.hpp>
#include <permutations/random_permutation_iterator.hpp>
#include <stern_attack/stern_algorithm.hpp>
#include <stern_attack/new_stern_algorithm.hpp>
#include <stern_attack/mmt_algorithm.hpp>
#include <stern_attack/fs-isd_algorithm.hpp>
#include <utils/benchmark.hpp>
#define assertm(exp, msg) assert(((void)msg, exp))
 

std::vector<std::string> ReadLinesFromFile(const std::string& file) {
    assertm(std::filesystem::exists(file), " is not found\n");

    std::ifstream ifs(file);
    std::vector<std::string> lines;
    std::string line;

    while(std::getline(ifs, line)) {
        lines.push_back(line);
    }

    return lines;
}


struct DataStruct {
    long duration;
    long numberOfIterations;
};


/**
 * @tparam DecodingStepAlgorithm type of algorithm (for example, ISD, Stern)
 * @param checkMatrix binary permuted check matrix for which gauss elimination should be applied
 * @param syndrome syndrome vector 
 * @param omega number of errors in codeword (i.e. number of ones in error vector)
 * @param algorithm algorithm for decoding 
 * @return number of iteration and time
 */
template <typename DecodingStepAlgorithm>
DataStruct DecodingBecnhmark(BinaryMatrix checkMatrix, boost::dynamic_bitset<> syndrome,
                                 const unsigned omega, const DecodingStepAlgorithm& algorithm){
    Timer timer(algorithm.algorithmName, 1, false);
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

            break;
        }
    }

    return DataStruct{timer.GetDuration(), numberOfIterations};
}

int main(int argc, const char** argv) {
    if(argc != 3) {
        std::cerr << "You should provide the name of output json and number of iterations\n";
        return 0;
    }

    std::ofstream output(argv[1]);
    int N = std::atoi(argv[2]);

    using json = nlohmann::json;

    json data;

    auto inputDataForMatrix = ReadLinesFromFile("benchmark/check_matrix.txt");
    auto inputDataForSyndrome = ReadLinesFromFile("benchmark/syndrome.txt");
    auto omega = 9;

    BinaryMatrix checkMatrix;
    for(auto& line: inputDataForMatrix) {
        std::reverse(line.begin(), line.end());
        checkMatrix.addRow(line);
    }

    std::reverse(inputDataForSyndrome.front().begin(), inputDataForSyndrome.front().end());
    boost::dynamic_bitset<> syndrome(inputDataForSyndrome.front());

    for(int iteration = 0; iteration < N; ++iteration) {
        auto resultStern = DecodingBecnhmark(checkMatrix, syndrome, omega, SternAlgorithm(checkMatrix.ColumnsSize()));
        auto resultSternHash = DecodingBecnhmark(checkMatrix, syndrome, omega, NewSternAlgorithm(checkMatrix.ColumnsSize()));
        auto resultFS = DecodingBecnhmark(checkMatrix, syndrome, omega, FS_ISD_Algorithm(checkMatrix.ColumnsSize()));
        auto resultMMT = DecodingBecnhmark(checkMatrix, syndrome, omega, MMTAlgorithm(checkMatrix.ColumnsSize(), checkMatrix.RowsSize()));

        data["stern"][iteration] = {
            {"iterations_count", resultStern.numberOfIterations},
            {"duration", resultStern.duration},
        };

        data["FS_ISD"][iteration] = {
            {"iterations_count", resultFS.numberOfIterations},
            {"duration", resultFS.duration},
        };

        data["MMT"][iteration] = {
            {"iterations_count", resultMMT.numberOfIterations},
            {"duration", resultMMT.duration},
        };

        data["stern_hash"][iteration] = {
            {"iterations_count", resultSternHash.numberOfIterations},
            {"duration", resultSternHash.duration},
        };
    }

    data["params"]["n"] = checkMatrix.ColumnsSize();
    data["params"]["k"] = checkMatrix.ColumnsSize() - checkMatrix.RowsSize();
    data["params"]["omega"] = omega;

    auto [sternP, sternL] = SternAlgorithm(checkMatrix.ColumnsSize()).getParams();
    auto [fsP, fsL] = FS_ISD_Algorithm(checkMatrix.ColumnsSize()).getParams();
    auto [mmtP, mmtL1, mmtL2, _] = MMTAlgorithm(checkMatrix.ColumnsSize(), checkMatrix.RowsSize()).getParams();

    data["stern_params"]["p"] = sternP;
    data["stern_params"]["l"] = sternL;
    data["stern_hash_params"]["p"] = sternP;
    data["stern_hash_params"]["l"] = sternL;
    data["FS_ISD_params"]["p"] = fsP;
    data["FS_ISD_params"]["l"] = fsL;
    data["MMT_params"]["p"] = mmtP;
    data["MMT_params"]["l1"] = mmtL1;
    data["MMT_params"]["l2"] = mmtL2;

    output << std::setw(4) << data << '\n';
}