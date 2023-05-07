find_package(Boost REQUIRED)
find_package(nlohmann_json REQUIRED)

set(programs
    "gauss_elimination"
    "sorting"
    "decoding"
)

set(test_data
    "check_matrix.txt"
    "syndrome.txt"
)

foreach(program ${programs})
    add_executable(${program} ${program}.cpp)
    target_include_directories(${program} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${program} PRIVATE Boost::thread)    
endforeach()

foreach(data ${test_data})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${data} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()