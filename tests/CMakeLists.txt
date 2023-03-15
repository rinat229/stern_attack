find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set(TESTS
    "algebra/binary_matrix"
    "permutations/primitive_permutation"
)

foreach(test ${TESTS})
    string(REPLACE / _ test_target_name test_${test})
    add_executable(${test_target_name} ${test}.cpp)
    target_include_directories(${test_target_name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_target_name} PRIVATE Boost::unit_test_framework)
endforeach()
