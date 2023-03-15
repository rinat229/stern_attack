cmake_minimum_required(VERSION 3.14)

project("SternAttack")

# set(INCLUDE_HEADERS 
#     "include/algebra/binary_matrix.hpp"
#     "include/permutations/primitive_permutation_iterator.hpp"
#     "include/permutations/permutation_base.hpp"
# )

# add_library(sternAttack INTERFACE ${INCLUDE_HEADERS})

add_subdirectory(tests)