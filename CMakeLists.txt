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

include_directories(src contrib util ${INCLUDE_DIR})
link_directories(${LIB_DIR})

option(WINDOWS_HIDE_CONSOLE "Whether to hide the console window in windows builds" ON)
option(DEBUG_OVERRIDE_CONSOLE "Whether to always show the console window in debug configuration" ON)

set(DBG_PREFIX $<$<CONFIG:Debug>:d>)

if(WIN32)
	if(WINDOWS_HIDE_CONSOLE)
		set(WIN32FLAG WIN32)
		add_definitions( -DHIDE_CONSOLE )
	else()
		set(WIN32FLAG "")
	endif()
	if(DEBUG_OVERRIDE_CONSOLE)
		add_definitions( -DOVERRIDE_CONSOLE )
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

set(ALL_SOURCE_FILES ${SOURCE_FILES} ${CMAKE_FILES})

add_executable(DeferredRenderer ${WIN32FLAG} ${ALL_SOURCE_FILES})
set_target_out_dir(DeferredRenderer ${OUT_DIR})

if(MSVC)
	set(INTR_FLAG "/Oi")
else()
	set(INTR_FLAG "")
endif()

target_compile_options(DeferredRenderer PUBLIC $<$<NOT:$<CONFIG:Debug>>:${INTR_FLAG}>)

add_subdirectory(conproc)

include(external_libs)

add_dependencies(DeferredRenderer boost glew glfw glm json luap)
target_link_libraries(DeferredRenderer ${EXT_LINK_LIBS} opengl32)

add_dependencies(conproc boost assimp json tiff squish)

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

set(ALL_CONTENT_FILES
	${ADDITIONAL_FILES}
	${OBJECT_FILES}
	${SHADER_FILES}
	${SCRIPT_FILES}
	${IMAGE_FILES}
	${MODEL_FILES}
)

foreach(content_file ${ALL_CONTENT_FILES})
	set(file_src ${CMAKE_CURRENT_SOURCE_DIR}/${content_file})
	
	set(dependees ${file_src})
	set(opt_file ${file_src}.opt)
	if(EXISTS ${opt_file})
		list(APPEND dependees ${opt_file})
	endif()
	
	get_filename_component(file_dst ${OUT_DIR}/${content_file} DIRECTORY)
	set(timestamp ${TIMESTAMP_DIR}/${content_file}.stamp)
	
	add_custom_command(
		OUTPUT ${timestamp}
		COMMAND ${CONPROC_COMMAND} ${file_src} -d ${file_dst}
		COMMAND ${CMAKE_COMMAND} -E touch ${timestamp}
		MAIN_DEPENDENCY ${file_src}
		DEPENDS ${dependees}
	)
	list(APPEND TIMESTAMPS ${timestamp})
endforeach()

add_custom_target(process_content ALL DEPENDS ${TIMESTAMPS})
add_dependencies(process_content conproc)
add_dependencies(DeferredRenderer process_content)

if(MSVC)
	if(MSVC_VERSION EQUAL 1700)
		set(TOOLVER 11.0)
	elseif(MSVC_VERSION EQUAL 1800)
		set(TOOLVER 12.0)
	elseif(MSVC_VERSION EQUAL 1900)
		set(TOOLVER 14.0)
	elseif(MSVC_VERSION EQUAL 1910)
		set(TOOLVER 14.1)
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
