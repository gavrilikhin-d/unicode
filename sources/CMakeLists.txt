add_library(
	unicode 
		utf8/compare.cpp
		layout.cpp
)
target_compile_features(unicode PUBLIC cxx_std_20)
target_link_libraries(unicode PRIVATE ${ICU_LIBRARIES})
target_include_directories(unicode PRIVATE ${ICU_INCLUDE_DIRS})
