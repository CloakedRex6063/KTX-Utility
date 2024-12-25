cmake_minimum_required(VERSION 3.30)
project(KTX-Utility)

set(CMAKE_CXX_STANDARD 23)

add_library(KTX-Utility Source/KtxUtility.cpp)
target_include_directories(KTX-Utility PUBLIC Include)

option(KtxWithTests "Enable unit tests" ON)
if (KtxWithTests)
    add_executable(KtxTestExec Test/test.cpp)
    target_link_libraries(KtxTestExec PRIVATE KTX-Utility)
    enable_testing()
    add_test(KTX_TEST COMMAND)
endif ()