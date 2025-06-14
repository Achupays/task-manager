cmake_minimum_required(VERSION 3.10)
project(TaskManagerGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к include
include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/include/nlohmannjson)

# Путь к SFML
set(SFML_DIR "C:/Program Files/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML 2.6.2 REQUIRED COMPONENTS graphics window system)

add_executable(TaskManager main.cpp)
target_link_libraries(TaskManager PRIVATE sfml-graphics sfml-window sfml-system)

# === GTest Setup ===
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# === Tests ===
enable_testing()

add_executable(user_tests
    tests/test_user.cpp
    src/user.cpp
    src/task.cpp
)
target_link_libraries(user_tests gtest gtest_main)
target_include_directories(user_tests PRIVATE include)
add_test(NAME UserTest COMMAND user_tests)