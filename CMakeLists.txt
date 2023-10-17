cmake_minimum_required(VERSION 3.15)
project(
    KThreadPool
    DESCRIPTION "std::thread pool"
    LANGUAGES CXX
)

include(cmake/CompileOptions.cmake)
include(cmake/Sanitizers.cmake)

add_subdirectory(core)
add_subdirectory(third_party/function2)

add_executable(
    ${PROJECT_NAME} main.cpp
)
target_link_libraries(
    ${PROJECT_NAME} ${PROJECT_NAME}_objs
)

include(FetchContent)
include(CTest)

add_subdirectory(test)
