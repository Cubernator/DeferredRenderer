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

option(WINDOWS_HIDE_CONSOLE "Whether to hide the console window in windows builds" OFF)

if(WIN32)
	if(WINDOWS_HIDE_CONSOLE)
		set(WIN32FLAG WIN32)
	else()
		set(WIN32FLAG "")
		add_definitions( -DHIDE_CONSOLE )
	endif()
endif()

add_executable(DeferredRenderer ${WIN32FLAG} ${ALL_SOURCE_FILES})
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

# TODO: create custom target for copying files to properly handle dependencies
# see here: http://cmake.3232098.n2.nabble.com/add-custom-command-OUTPUT-does-not-accept-generator-expressions-why-tp7593126p7593129.html

foreach(lib_file ${SHARED_LIB_FILES})
	get_filename_component(lib_filename ${lib_file} NAME)
	add_custom_command(
		TARGET DeferredRenderer POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${lib_file} $<TARGET_FILE_DIR:DeferredRenderer>/${lib_filename}
	)
endforeach()

foreach(content_file ${CONTENT_FILES})
	set(file_src ${CMAKE_CURRENT_SOURCE_DIR}/${content_file})
	add_custom_command(
		TARGET DeferredRenderer POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file_src} $<TARGET_FILE_DIR:DeferredRenderer>/${content_file}
	)
endforeach()

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
