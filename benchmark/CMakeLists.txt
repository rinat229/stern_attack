set(programs
    "gauss_elimination"
)

foreach(program ${programs})
    add_executable(${program} ${program}.cpp)
    target_include_directories(${program} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${program} PRIVATE Boost::thread)    
endforeach()
