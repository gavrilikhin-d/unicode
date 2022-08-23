find_package(benchmark REQUIRED)

include_directories(${ICU_INCLUDE_DIRS})

add_executable(english_benchmark english.cpp)
target_link_libraries(
	english_benchmark 
		benchmark::benchmark 
		unicode 
		${ICU_LIBRARIES}
)

add_executable(wiki_benchmark wiki.cpp)
target_link_libraries(
	wiki_benchmark 
		benchmark::benchmark 
		unicode 
		${ICU_LIBRARIES}
)

add_executable(iteration_benchmark iteration.cpp)
target_link_libraries(
	iteration_benchmark 
	benchmark::benchmark 
	unicode 
	${ICU_LIBRARIES}
)