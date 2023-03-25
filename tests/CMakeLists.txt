find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(Boost COMPONENTS thread REQUIRED)



set(TESTS
    "algebra/binary_matrix"

    "permutations/primitive_permutation"
    "permutations/combination"

    "stern_attack/information_set_decoding"
)

enable_testing()

foreach(test ${TESTS})
    string(REPLACE / _ test_target_name test_${test})
    add_executable(${test_target_name} ${test}.cpp)
    target_include_directories(${test_target_name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_target_name} PRIVATE Boost::unit_test_framework Boost::thread)    
    target_compile_definitions(${test_target_name} PUBLIC TEST_DATA=${CMAKE_CURRENT_BINARY_DIR}/test_data)
endforeach()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
