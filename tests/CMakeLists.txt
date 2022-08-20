enable_testing()

find_package(GTest REQUIRED)

add_executable(test test.cpp)
target_link_libraries(test String GTest::gtest GTest::gtest_main)

add_executable(view view.cpp)
target_link_libraries(view unicode GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test)
gtest_discover_tests(view)