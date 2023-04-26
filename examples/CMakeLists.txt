set(test_sets
    "decoding_challenge_1041"
    "decoding_challenge_381"
)

add_executable(example_cracking example_cracking.cpp)
target_include_directories(example_cracking PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(example_cracking PRIVATE Boost::thread)    

foreach(test_set ${test_sets})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${test_set} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

