cmake_minimum_required(VERSION 3.15)

project("SternAttack")


find_package(Boost COMPONENTS thread REQUIRED)
add_subdirectory(tests)