cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(DeferredRenderer CXX C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

include(source_files)

set(ADDITIONAL_FILES "app-info.json")

option(WINDOWS_HIDE_CONSOLE "Whether to hide the console window in windows builds" OFF)

if(WIN32)
	if(WINDOWS_HIDE_CONSOLE)
		set(WIN32FLAG WIN32)
	else()
		set(WIN32FLAG "")
		add_definitions( -DHIDE_CONSOLE )
	endif()
endif()

add_executable(DeferredRenderer ${WIN32FLAG} ${ALL_SOURCE_FILES} ${ADDITIONAL_FILES})

set(LINK_LIBRARIES glew32 glfw3dll opengl32 assimp)

target_link_libraries(DeferredRenderer ${LINK_LIBRARIES})

include(external_libs)

if(MSVC)
	set(SHARED_LIB_FILES
		${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/glfw3.dll
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/glew32.dll
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assimp.dll
	)
else()
	# TODO: set proper values here
	set(SHARED_LIB_FILES "")
endif()

set(OUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)

set(TIMESTAMPS "")
set(TIMESTAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/content.dir)

foreach(lib_file ${SHARED_LIB_FILES})
	get_filename_component(lib_filename ${lib_file} NAME)
	set(lib_file_dst ${OUT_DIR}/${lib_filename})
	set(timestamp ${TIMESTAMP_DIR}/${lib_filename}.stamp)
	add_custom_command(
		OUTPUT ${timestamp}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${lib_file} ${lib_file_dst}
		COMMAND ${CMAKE_COMMAND} -E touch ${timestamp}
		DEPENDS ${lib_file}
	)
	list(APPEND TIMESTAMPS ${timestamp})
endforeach()

set(ALL_CONTENT_FILES ${SHADER_FILES} ${CONTENT_FILES} ${ADDITIONAL_FILES})

foreach(content_file ${ALL_CONTENT_FILES})
	set(file_src ${CMAKE_CURRENT_SOURCE_DIR}/${content_file})
	set(file_dst ${OUT_DIR}/${content_file})
	set(timestamp ${TIMESTAMP_DIR}/${content_file}.stamp)
	add_custom_command(
		OUTPUT ${timestamp}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file_src} ${file_dst}
		COMMAND ${CMAKE_COMMAND} -E touch ${timestamp}
		DEPENDS ${file_src}
	)
	list(APPEND TIMESTAMPS ${timestamp})
endforeach()

add_custom_target(copy_content_files ALL DEPENDS ${TIMESTAMPS})
add_dependencies(DeferredRenderer copy_content_files)

if(MSVC)
	if(MSVC11)
		set(TOOLVER 11.0)
	elseif(MSVC12)
		set(TOOLVER 12.0)
	elseif(MSVC14)
		set(TOOLVER 14.0)
	else()
		set(TOOLVER "")
	endif()
	
	set(USERFILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user)
	
	file(WRITE ${USERFILE}
"<?xml version=\"1.0\" encoding=\"utf-8\"?>
<Project ToolsVersion=\"${TOOLVER}\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
	
	foreach(configuration ${CMAKE_CONFIGURATION_TYPES})
		file(APPEND ${USERFILE}
"  <PropertyGroup Condition=\"'$(Configuration)'=='${configuration}'\">
    <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>\n" )
	endforeach()
	
	file(APPEND ${USERFILE} "</Project>\n")
endif()
