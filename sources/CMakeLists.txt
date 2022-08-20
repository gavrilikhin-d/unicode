# ICU dependency
find_package(ICU 70 COMPONENTS uc i18n REQUIRED)
while (NOT EXISTS ${ICU_INCLUDE_DIRS}/unicode/brkiter.h)
	# TODO: fallback to other package
	message(FATAL_ERROR "ICU include directory doesn't contain brkiter.h")
endwhile()

add_library(
	unicode 
		utf8/compare.cpp
)
target_compile_features(unicode PUBLIC cxx_std_20)
target_link_libraries(unicode PRIVATE ${ICU_LIBRARIES})
target_include_directories(unicode PRIVATE ${ICU_INCLUDE_DIRS})
