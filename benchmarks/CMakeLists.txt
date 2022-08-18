find_package(benchmark REQUIRED)
add_executable(english english.cpp)
target_link_libraries(english benchmark::benchmark String)