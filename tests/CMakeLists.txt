enable_testing()

find_package(GTest REQUIRED)

add_executable(wiki_test wiki.cpp)
target_link_libraries(
	wiki_test
		unicode 
		GTest::gtest GTest::gtest_main 
		${ICU_LIBRARIES}
)

add_executable(view_test view.cpp)
target_link_libraries(
	view_test
		unicode 
		GTest::gtest GTest::gtest_main 
		${ICU_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(wiki_test)
gtest_discover_tests(view_test)