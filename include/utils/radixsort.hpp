#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <array>
#include <iterator>
#include <iostream>
#include <functional>


namespace std{
    template <typename CharT, typename Traits, typename ValueType>
    std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>& os, const std::vector<ValueType>& b){
        for(const auto& value: b){
            os << value << ' ';
        }

        return os;
    }
}

using CollisionType = std::pair<boost::dynamic_bitset<>, std::vector<unsigned>>;

template <typename IteratorType, typename ArgumentType = std::identity>
void Radixsort(IteratorType begin, IteratorType end, ArgumentType argument = ArgumentType()) {
    using ValueType = IteratorType::value_type;
    using BitsetType = decltype(argument(*begin));

    long int n = end - begin;
    unsigned keyLength = argument(*begin).size();

    std::vector<ValueType> tempVec(end - begin);

    for(int bitIdx = keyLength - 1; bitIdx >= 0; --bitIdx) {
        std::array<long int, 2> count = {0, std::count_if(begin, end, [&argument, &bitIdx](auto& val){
                                return argument(val)[bitIdx] == false;
                            })};

        for(auto iter = begin; iter != end; ++iter) {
            size_t val = argument(*iter)[bitIdx];
            tempVec[count[val]] = *iter;
            ++count[val];
        }

        // std::cout << tempVec << '\n';

        unsigned tempIdx = 0;

        for(auto iter = begin; iter != end; ++iter) {
            std::swap(*iter, tempVec[tempIdx++]);
        }
    }
}