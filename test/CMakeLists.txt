FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(utils)

add_executable(
    ${PROJECT_NAME}_test
    thread_pool_test.cpp
    mpmc_queue_test.cpp
    wait_group_test.cpp
)

target_include_directories(
    ${PROJECT_NAME}_test PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}_test
    GTest::gtest_main
    ${PROJECT_NAME}_objs
    ${PROJECT_NAME}_utils
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)
