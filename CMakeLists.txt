# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Tan Li Boon

cmake_minimum_required(VERSION 3.20.0)

project(favonius LANGUAGES CXX VERSION 0.1.0)

option(ALLOW_STD_HEADERS "Allow usage of headers from the C++ standard library." OFF)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if (CONFIG_LIBFAVONIUS)
    file(GLOB favonius_sources "${CMAKE_CURRENT_SOURCE_DIR}/source/*")
    zephyr_library()
    zephyr_include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
    zephyr_library_sources(
        ${favonius_sources}
    )
    zephyr_compile_definitions("FAVONIUS_ALLOW_STD_HEADERS=$<BOOL:${ALLOW_STD_HEADERS}>")
endif()

if (CONFIG_ZTEST)
    file(GLOB favonius_sources "${CMAKE_CURRENT_SOURCE_DIR}/test/*")
    # TODO How to add tests?
endif()