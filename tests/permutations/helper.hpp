#pragma once

#include <vector>
#include <iostream>

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