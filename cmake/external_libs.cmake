include(ExternalProject)
include(CMakeParseArguments)

set(EXTERNAL_DIR		${CMAKE_BINARY_DIR}/externals)
set(EXT_TMP_DIR			${EXTERNAL_DIR}/tmp)
set(EXT_STAMP_DIR		${EXTERNAL_DIR}/stamp)
set(EXT_DOWNLOAD_DIR	${EXTERNAL_DIR}/download)
set(EXT_SOURCE_DIR		${EXTERNAL_DIR}/src)
set(EXT_BINARY_DIR		${EXTERNAL_DIR}/build)
set(EXT_INSTALL_DIR		${CMAKE_BINARY_DIR})

set(EXT_CM_INSTALL_PREFIX -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR})

function(add_project name)
	set(MVARGS COMMANDS CMAKE_ARGS)
	cmake_parse_arguments(ap "" "" "${MVARGS}" ${ARGN})
	
	ExternalProject_Add(
		${name}
		
		TMP_DIR			${EXT_TMP_DIR}
		STAMP_DIR		${EXT_STAMP_DIR}
		DOWNLOAD_DIR	${EXT_DOWNLOAD_DIR}
		SOURCE_DIR		${EXT_SOURCE_DIR}/${name}
		BINARY_DIR		${EXT_BINARY_DIR}/${name}
		INSTALL_DIR		${EXT_INSTALL_DIR}
		
		UPDATE_COMMAND ""
		
		CMAKE_ARGS ${EXT_CM_INSTALL_PREFIX} ${ap_CMAKE_ARGS}
		
		${ap_COMMANDS}
	)
	add_dependencies(DeferredRenderer ${name})
endfunction(add_project)

set(DUMMY_CMD ${CMAKE_COMMAND} -E echo "Placeholder command!")

add_project(glew COMMANDS
	URL "http://downloads.sourceforge.net/project/glew/glew/2.0.0/glew-2.0.0.tgz"
	CONFIGURE_COMMAND "${CMAKE_COMMAND}" ../../src/glew/build/cmake/ -G "${CMAKE_GENERATOR}" ${EXT_CM_INSTALL_PREFIX} -DBUILD_UTILS=OFF
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

add_project(json COMMANDS
	GIT_REPOSITORY "https://github.com/nlohmann/json.git"
	GIT_TAG "v2.0.6"
	CMAKE_ARGS -DBuildTests=OFF
)

include(add_boost)