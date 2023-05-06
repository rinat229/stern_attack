cmake_minimum_required(VERSION 3.15)

project("SternAttack")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-frecord-gcc-switches")
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Boost COMPONENTS thread REQUIRED)
add_subdirectory(tests)
add_subdirectory(benchmark)
add_subdirectory(examples)