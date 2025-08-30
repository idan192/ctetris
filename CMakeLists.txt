cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(C_STANDARD 23)
set(C_STANDARD_REQUIRED ON)
set(PROJECT_NAME tetris)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_VERSION 1.0.0)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_CLANG_FORMAT "clang-format -style=file")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -DDEBUG -Wall -Wextra -Wpedantic -fsanitize=address,undefined")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_link_options(-fsanitize=address,undefined)
endif()

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

# Dependencies
find_package(SDL3 REQUIRED)

# Managed Project Dependencies
include(FetchContent)
FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG v2.6.1
)
FetchContent_MakeAvailable(unity)

# Configuration
configure_file(src/cmake_variables.h.in ${CMAKE_SOURCE_DIR}/src/_gen/cmake_variables.h @ONLY)

set(LIB_HEADERS
  src/game.h ${CMAKE_SOURCE_DIR}/src/_gen/cmake_variables.h)

set(LIB_SOURCES
  src/game.c ${LIB_HEADERS})

add_library(${PROJECT_NAME}_lib STATIC ${LIB_SOURCES})
target_include_directories(${PROJECT_NAME}_lib PRIVATE ${SDL3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME}_lib ${SDL3_LIBRARIES})

# Executable
set(SOURCES
  src/main.c)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

# Tests
enable_testing()

set(TEST_SOURCES
  test/test_game.c
)

add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})
target_include_directories(${PROJECT_NAME}_test PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/test
  ${unity_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/_gen
  ${CMAKE_SOURCE_DIR}/src  
)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_lib unity)

add_test(NAME UnitTests COMMAND ${PROJECT_NAME}_test)

# Assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
