#include <chrono>
#include <iostream>

class Timer {
public: 
    Timer(const std::string& nameOfProgram, unsigned numberOfIterations = 1, bool printDuration = true) : start_point(std::chrono::high_resolution_clock::now()), 
    numberOfIterations(numberOfIterations), nameOfProgram(nameOfProgram), printDuration(printDuration) {}

    long GetDuration() {
        auto end_point = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::milliseconds> (start_point).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::milliseconds> (end_point).time_since_epoch().count();

        return end - start;
    }

    ~Timer(){

        auto duration = GetDuration();
        if(printDuration){
            std::cout << "The program \"" << nameOfProgram << "\" took " << duration << "ms";
            if(numberOfIterations > 1) {
                std::cout << " on " << numberOfIterations << " iterations";
            }
            std::cout << '\n';
        }
    }

    const std::string& name(){
        return nameOfProgram;
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_point;
    std::string nameOfProgram;
    unsigned numberOfIterations;
    bool printDuration;
};