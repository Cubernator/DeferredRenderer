include(ExternalProject)
include(CMakeParseArguments)

set(EXTERNAL_DIR externals)

function(add_project name)
	set(MVARGS COMMANDS CMAKE_ARGS)
	cmake_parse_arguments(ap "" "" "${MVARGS}" ${ARGN})
	
	ExternalProject_Add(
		${name}
		
		TMP_DIR ${EXTERNAL_DIR}/tmp
		STAMP_DIR ${EXTERNAL_DIR}/stamp
		DOWNLOAD_DIR ${EXTERNAL_DIR}/download
		SOURCE_DIR ${EXTERNAL_DIR}/src/${name}
		BINARY_DIR ${EXTERNAL_DIR}/build/${name}
		INSTALL_DIR .
		
		UPDATE_COMMAND ""
		
		CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} ${ap_CMAKE_ARGS}
		
		${ap_COMMANDS}
	)
	add_dependencies(DeferredRenderer ${name})
endfunction(add_project)

set(DUMMY_CMD ${CMAKE_COMMAND} -E echo "Placeholder command!")

add_project(glew COMMANDS
	URL "http://downloads.sourceforge.net/project/glew/glew/2.0.0/glew-2.0.0.tgz"
	CONFIGURE_COMMAND "${CMAKE_COMMAND}" ../../src/glew/build/cmake/ -G "${CMAKE_GENERATOR}" -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} -DBUILD_UTILS=OFF
	BUILD_COMMAND "${CMAKE_COMMAND}" --build . --config Release
	INSTALL_COMMAND "${CMAKE_COMMAND}" --build . --config Release --target install
)

add_project(glfw COMMANDS
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.2.1"
	CMAKE_ARGS -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF
)

add_project(glm COMMANDS
	GIT_REPOSITORY "https://github.com/g-truc/glm.git"
	GIT_TAG "0.9.8.1"
)
