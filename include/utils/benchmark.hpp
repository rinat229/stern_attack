#include <chrono>
#include <iostream>

class Timer
{
public: 
    Timer(const std::string& nameOfProgram, unsigned numberOfIterations = 1) : start_point(std::chrono::high_resolution_clock::now()), 
    numberOfIterations(numberOfIterations), nameOfProgram(nameOfProgram) {}

    ~Timer(){
        auto end_point = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::microseconds> (start_point).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds> (end_point).time_since_epoch().count();

        auto duration = end - start;
        std::cout << "The program \"" << nameOfProgram << "\" took " << duration << "us";
        if(numberOfIterations > 1) {
            std::cout << " on " << numberOfIterations << " iterations";
        }
        std::cout << '\n';
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_point;
    std::string nameOfProgram;
    unsigned numberOfIterations;
};