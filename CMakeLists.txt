cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

project(DeferredRenderer CXX C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

include(source_files)

add_executable(DeferredRenderer WIN32 ${ALL_SOURCE_FILES})
target_compile_features(DeferredRenderer PUBLIC cxx_lambdas)

target_link_libraries(DeferredRenderer glew32 glfw3dll opengl32)

include(external_libs)

if(MSVC)
	set(SHARED_LIB_FILES
		${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/glfw3.dll
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/glew32.dll
	)
else()
	set(SHARED_LIB_FILES "")
endif()

foreach(lib_file ${SHARED_LIB_FILES})
	get_filename_component(lib_filename ${lib_file} NAME)
	add_custom_command(
		TARGET DeferredRenderer POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${lib_file} $<TARGET_FILE_DIR:DeferredRenderer>/${lib_filename}
	)
endforeach()
