cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(DeferredRenderer CXX C)

set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(LIB_DIR ${CMAKE_BINARY_DIR}/lib)
set(INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)

include(source_files)

set(ADDITIONAL_FILES "app-info.json")

include_directories(${INCLUDE_DIR} include)
link_directories(${LIB_DIR})

option(WINDOWS_HIDE_CONSOLE "Whether to hide the console window in windows builds" OFF)

set(DBG_PREFIX $<$<CONFIG:Debug>:d>)

if(WIN32)
	if(WINDOWS_HIDE_CONSOLE)
		set(WIN32FLAG WIN32)
		add_definitions( -DHIDE_CONSOLE )
	else()
		set(WIN32FLAG "")
	endif()
endif()

macro(set_target_out_dir target dir)
	set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${dir})
	foreach(configuration ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${configuration} config)
		set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${config} ${dir})
	endforeach()
endmacro()

add_definitions( -DBOOST_ALL_NO_LIB )

set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/DeferredRenderer)

add_executable(DeferredRenderer ${WIN32FLAG} ${ALL_SOURCE_FILES} ${ADDITIONAL_FILES})
set_target_out_dir(DeferredRenderer ${OUT_DIR})

add_subdirectory(conproc)

include(external_libs)

add_dependencies(DeferredRenderer boost glew glfw glm json tiff)
target_link_libraries(DeferredRenderer ${EXT_LINK_LIBS} opengl32)

add_dependencies(conproc boost assimp json)

foreach(lib_file ${EXT_SHARED_LIBS})
	set(lib_file_src ${BIN_DIR}/${lib_file})
	set(lib_file_dst ${OUT_DIR}/${lib_file})
	add_custom_command(
		TARGET DeferredRenderer POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${lib_file_src} ${lib_file_dst}
	)
endforeach()


set(TIMESTAMPS "")
set(TIMESTAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/process_content.dir)

set(ALL_CONTENT_FILES ${SHADER_FILES} ${CONTENT_FILES} ${ADDITIONAL_FILES})

foreach(content_file ${ALL_CONTENT_FILES})
	set(file_src ${CMAKE_CURRENT_SOURCE_DIR}/${content_file})
	get_filename_component(file_dst ${OUT_DIR}/${content_file} DIRECTORY)
	set(timestamp ${TIMESTAMP_DIR}/${content_file}.stamp)
	add_custom_command(
		OUTPUT ${timestamp}
		COMMAND ${CONPROC_COMMAND} ${file_src} -d ${file_dst}
		COMMAND ${CMAKE_COMMAND} -E touch ${timestamp}
		DEPENDS ${file_src}
	)
	list(APPEND TIMESTAMPS ${timestamp})
endforeach()

add_custom_target(process_content ALL DEPENDS ${TIMESTAMPS})
add_dependencies(process_content conproc)
add_dependencies(DeferredRenderer process_content)

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
	
	set(USERFILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user)
	
	file(WRITE ${USERFILE}
"<?xml version=\"1.0\" encoding=\"utf-8\"?>
<Project ToolsVersion=\"${TOOLVER}\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n
	
  <PropertyGroup>
    <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>\n
</Project>\n")
endif()
