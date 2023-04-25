cmake_minimum_required(VERSION 3.15)

project("SternAttack")

set (CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)
find_package(Boost COMPONENTS thread REQUIRED)
add_subdirectory(tests)