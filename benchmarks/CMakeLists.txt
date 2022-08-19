find_package(benchmark REQUIRED)

add_executable(english english.cpp)
target_link_libraries(english benchmark::benchmark String)

add_executable(wiki wiki.cpp)
target_link_libraries(wiki benchmark::benchmark String)

add_executable(iteration iteration.cpp)
target_link_libraries(iteration benchmark::benchmark String)