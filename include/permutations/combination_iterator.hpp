#include <string>


class Combination {
    std::string bitmask;

public:
    Combination(int n, int k): bitmask(k, 1) {
        bitmask.resize(n, 0);
    }

    class CombinationIterator {
        
    }
};