cmake_minimum_required(VERSION 3.1)

#---------------------------------------------------------------------------------------
# Set default build type to release
#---------------------------------------------------------------------------------------
if(UNIX)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
    endif()
endif()

#---------------------------------------------------------------------------------------
# Compiler config
#---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


#---------------------------------------------------------------------------------------
# 3rd cmake modules
#---------------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#---------------------------------------------------------------------------------------
# Self defined postfix
#---------------------------------------------------------------------------------------
set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "Set debug library postfix")

# #---------------------------------------------------------------------------------------
# # Windows platform configuration types
# #---------------------------------------------------------------------------------------
# if(WIN32)
#     set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Just limit the Debug/Release configuration" FORCE)
# endif()

#---------------------------------------------------------------------------------------
# Coverage reporting
#---------------------------------------------------------------------------------------
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)

#---------------------------------------------------------------------------------------
# ARK project
#---------------------------------------------------------------------------------------
include(cmake/utils.cmake)
ark_extract_version()

project(ark VERSION ${ARK_VERSION} LANGUAGES CXX)
message(STATUS "Build ark: v${ARK_VERSION}")

set(ROOT_DIR ${PROJECT_SOURCE_DIR})

include_directories(
    ${ROOT_DIR}
    ${ROOT_DIR}/dep
    ${ROOT_DIR}/dep/common
    ${ROOT_DIR}/dep/protobuf/src
    ${ROOT_DIR}/dep/brynet/src
    ${ROOT_DIR}/dep/spdlog/include
    ${ROOT_DIR}/src
    ${ROOT_DIR}/src/plugin
    ${ROOT_DIR}/src/server)

set(BIN_OUTPUT_DIR ${ROOT_DIR}/bin/bin)
set(DEP_LIB_DIR ${ROOT_DIR}/dep/lib/ ${BIN_OUTPUT_DIR})

if(UNIX)
    SET(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -ggdb -Wall -Wextra -D_DEBUG -fPIC")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ggdb -Wall -Wextra -DNDEBUG -fPIC")
    
    if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    endif()
    add_definitions(-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-strict-aliasing -Wno-pmf-conversions -Wignored-qualifiers)
    
    if(ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        SET(CMAKE_CXX_FLAGS_DEBUG "-lgcov --coverage")
        SET(CMAKE_CXX_FLAGS_RELEASE "-lgcov --coverage")
        message("Build with coverage")
        add_compile_options(--coverage -O0)
    endif()
else(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
endif()

link_directories(${DEP_LIB_DIR})

#---------------------------------------------------------------------------------------
# log
#---------------------------------------------------------------------------------------
message(STATUS "ROOT_DIR=${ROOT_DIR}")
message(STATUS "DEP_LIB_DIR=${DEP_LIB_DIR}")
message(STATUS "BIN_OUTPUT_DIR=${BIN_OUTPUT_DIR}")
message(STATUS "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

#---------------------------------------------------------------------------------------
# codecov
#---------------------------------------------------------------------------------------
find_package(codecov)

#---------------------------------------------------------------------------------------
# sub projects
#---------------------------------------------------------------------------------------
message(STATUS "Start to build all ...")

add_subdirectory(src/app)
add_subdirectory(src/proto)
add_subdirectory(src/plugin)
add_subdirectory(src/server)

if(BUILD_SAMPLES)
    message(STATUS "Buiding samples ...")
    add_subdirectory(samples)    
endif(BUILD_SAMPLES)

if(BUILD_TESTS)
    message(STATUS "Buiding tests ...")
    enable_testing()
    add_subdirectory(test)
endif(BUILD_TESTS)
