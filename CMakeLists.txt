cmake_minimum_required(VERSION 3.20.0)

# This must come BEFORE project()
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(favonius LANGUAGES CXX VERSION 0.1.0)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

file(GLOB favonius_sources "${CMAKE_CURRENT_SOURCE_DIR}/source/*")
zephyr_library()
zephyr_include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
zephyr_library_sources(
    ${favonius_sources}
)