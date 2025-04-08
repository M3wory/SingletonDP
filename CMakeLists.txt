cmake_minimum_required(VERSION 3.14)
project(Singleton VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the main executable
add_executable(singleton_demo src/main.cpp)

# Enable threading support
find_package(Threads REQUIRED)
target_link_libraries(singleton_demo PRIVATE Threads::Threads)

# Testing with Catch2
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    # FetchContent for Catch2
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.3.2 # Replace with latest version if needed
    )
    FetchContent_MakeAvailable(Catch2)

    # Add test executable
    add_executable(singleton_tests tests/test.cpp)
    target_link_libraries(singleton_tests PRIVATE Catch2::Catch2WithMain Threads::Threads)

    # Enable CTest integration
    include(CTest)
    include(Catch)
    catch_discover_tests(singleton_tests)
endif()

# Installation targets
install(TARGETS singleton_demo DESTINATION bin)
if(BUILD_TESTS)
    install(TARGETS singleton_tests DESTINATION bin)
endif()