#define BOOST_TEST_MODULE InformationSetDecoding
#define NUMBER_OF_SMALL_TEST_CASES 2
#define NUMBER_OF_MEDIUM_TEST_CASES 4
#define NUMBER_OF_BIG_TEST_CASES 2

#include <algebra/binary_matrix.hpp>
#include <stern_attack/information_set_decoding.hpp>
#include <stern_attack/stern_algorithm.hpp>
#include <stern_attack/base_decoding.hpp>
#include "helper.hpp"
#include <chrono>

#include <boost/test/included/unit_test.hpp>
#include <algorithm>
#include <map>


BOOST_AUTO_TEST_CASE(InformationSetDecodingSmall) {
    std::map<int, int> numberOfErrors = {
        {1, 2},
        {2, 2},
    };

    for(const auto& [i, omega]: numberOfErrors){
        std::string nameOfDirectory("small");
        nameOfDirectory += std::to_string(i) + "/";
        auto inputDataForMatrix = ReadLinesFromFile(nameOfDirectory + "check_matrix.txt");
        auto inputDataForSyndrome = ReadLinesFromFile(nameOfDirectory + "syndrome.txt");
        auto inputDataForErrorVector = ReadLinesFromFile(nameOfDirectory + "error_vector.txt");

        BOOST_TEST(inputDataForMatrix.size() == inputDataForSyndrome.front().size());
        BOOST_TEST(inputDataForMatrix.front().size() == inputDataForErrorVector.front().size());

        BinaryMatrix checkMatrix;
        for(auto& line: inputDataForMatrix) {
            std::reverse(line.begin(), line.end());
            checkMatrix.addRow(line);
        }

        std::reverse(inputDataForSyndrome.front().begin(), inputDataForSyndrome.front().end());
        boost::dynamic_bitset<> syndrome(inputDataForSyndrome.front());

        std::reverse(inputDataForErrorVector.front().begin(), inputDataForErrorVector.front().end());
        boost::dynamic_bitset<> errorVector(inputDataForErrorVector.front());
        auto begin = std::chrono::steady_clock::now();
        boost::dynamic_bitset<> errorVectorFromISD = Decoding(checkMatrix, syndrome, omega, InformationSetDecodingStep);
        boost::dynamic_bitset<> errorVectorFromStern = Decoding(checkMatrix, syndrome, omega, SternAlgorithmStep);
        
        BOOST_TEST(errorVector == errorVectorFromISD);
        BOOST_TEST(errorVector == errorVectorFromStern);
    
    }
}

BOOST_AUTO_TEST_CASE(InformationSetDecodingMedium) {
    std::map<int, int> numberOfErrors = {
        {1, 2},
        {2, 4},
        {3, 6},
        {4, 7},
    };

    for(const auto& [i, omega]: numberOfErrors){
        std::string nameOfDirectory("medium");
        nameOfDirectory += std::to_string(i) + "/";
        auto inputDataForMatrix = ReadLinesFromFile(nameOfDirectory + "check_matrix.txt");
        auto inputDataForSyndrome = ReadLinesFromFile(nameOfDirectory + "syndrome.txt");
        auto inputDataForErrorVector = ReadLinesFromFile(nameOfDirectory + "error_vector.txt");

        BOOST_TEST(inputDataForMatrix.size() == inputDataForSyndrome.front().size());
        BOOST_TEST(inputDataForMatrix.front().size() == inputDataForErrorVector.front().size());

        BinaryMatrix checkMatrix;
        for(auto& line: inputDataForMatrix) {
            std::reverse(line.begin(), line.end());
            checkMatrix.addRow(line);
        }

        std::reverse(inputDataForSyndrome.front().begin(), inputDataForSyndrome.front().end());
        boost::dynamic_bitset<> syndrome(inputDataForSyndrome.front());

        std::reverse(inputDataForErrorVector.front().begin(), inputDataForErrorVector.front().end());
        boost::dynamic_bitset<> errorVector(inputDataForErrorVector.front());
        auto begin = std::chrono::steady_clock::now();
        boost::dynamic_bitset<> errorVectorFromISD = Decoding(checkMatrix, syndrome, omega, InformationSetDecodingStep);
        boost::dynamic_bitset<> errorVectorFromStern = Decoding(checkMatrix, syndrome, omega, SternAlgorithmStep);
        auto end = std::chrono::steady_clock::now();
        // auto begin_parallel = std::chrono::steady_clock::now();
        // boost::dynamic_bitset<> errorVectorFromISDParallel = InformationSetDecodingParallel(checkMatrix, syndrome, omega, 5);
        // auto end_parallel = std::chrono::steady_clock::now();

        // std::cout << "Time default algo - " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
        // std::cout << "Time parallel algo - " << std::chrono::duration_cast<std::chrono::milliseconds>(end_parallel - begin_parallel).count() << std::endl;

        BOOST_TEST(errorVector == errorVectorFromISD);
        BOOST_TEST(errorVector == errorVectorFromStern);
    }
}

BOOST_AUTO_TEST_CASE(InformationSetDecodingBig) {
    std::map<int, int> numberOfErrors = {
        {1, 12},
        {2, 15},
    };

    for(const auto& [i, omega]: numberOfErrors){
        std::string nameOfDirectory("big");
        nameOfDirectory += std::to_string(i) + "/";
        auto inputDataForMatrix = ReadLinesFromFile(nameOfDirectory + "check_matrix.txt");
        auto inputDataForSyndrome = ReadLinesFromFile(nameOfDirectory + "syndrome.txt");
        auto inputDataForErrorVector = ReadLinesFromFile(nameOfDirectory + "error_vector.txt");

        BOOST_TEST(inputDataForMatrix.size() == inputDataForSyndrome.front().size());
        BOOST_TEST(inputDataForMatrix.front().size() == inputDataForErrorVector.front().size());

        BinaryMatrix checkMatrix;
        for(auto& line: inputDataForMatrix) {
            std::reverse(line.begin(), line.end());
            checkMatrix.addRow(line);
        }

        std::reverse(inputDataForSyndrome.front().begin(), inputDataForSyndrome.front().end());
        boost::dynamic_bitset<> syndrome(inputDataForSyndrome.front());

        std::reverse(inputDataForErrorVector.front().begin(), inputDataForErrorVector.front().end());
        boost::dynamic_bitset<> errorVector(inputDataForErrorVector.front());
        auto begin = std::chrono::steady_clock::now();
        // boost::dynamic_bitset<> errorVectorFromISD = Decoding(checkMatrix, syndrome, omega, InformationSetDecodingStep);
        boost::dynamic_bitset<> errorVectorFromStern = Decoding(checkMatrix, syndrome, omega, SternAlgorithmStep);
        auto end = std::chrono::steady_clock::now();

        // auto begin_parallel = std::chrono::steady_clock::now();
        // boost::dynamic_bitset<> errorVectorFromISDParallel = InformationSetDecodingParallel(checkMatrix, syndrome, omega, 5);
        // auto end_parallel = std::chrono::steady_clock::now();

        // std::cout << "Time default algo - " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
        // std::cout << "Time parallel algo - " << std::chrono::duration_cast<std::chrono::milliseconds>(end_parallel - begin_parallel).count() << std::endl;

        // BOOST_TEST(errorVector == errorVectorFromISD);
        BOOST_TEST(errorVector == errorVectorFromStern);
    }
} 
