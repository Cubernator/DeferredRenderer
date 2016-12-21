include(ExternalProject)
include(CMakeParseArguments)

set(EXTERNAL_DIR		${CMAKE_BINARY_DIR}/externals)
set(EXT_TMP_DIR			${EXTERNAL_DIR}/tmp)
set(EXT_STAMP_DIR		${EXTERNAL_DIR}/stamp)
set(EXT_DOWNLOAD_DIR	${EXTERNAL_DIR}/download)
set(EXT_SOURCE_DIR		${EXTERNAL_DIR}/src)
set(EXT_BINARY_DIR		${EXTERNAL_DIR}/build)
set(EXT_INSTALL_DIR		${CMAKE_BINARY_DIR})

set(EXT_INCLUDE_DIR ${EXT_INSTALL_DIR}/include)

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
endfunction(add_project)

set(DUMMY_CMD ${CMAKE_COMMAND} -E echo "Placeholder command!")

set(TIFF_NAME tiff${DBG_PREFIX})
set(ZLIB_NAME zlib${DBG_PREFIX}1)

# HACK: using a relative path to the include directory as a workaround, because find_package tends to return incorrect paths
set(ZLIB_PKG_ARGS -DZLIB_ROOT:path=${EXT_INSTALL_DIR} -DZLIB_INCLUDE_DIR:path=../../../include)

if(WIN32)
	set(GLFW_SHARED_LIB "glfw3.dll")
	set(GLEW_SHARED_LIB "glew32.dll")
	set(TIFF_SHARED_LIB "${TIFF_NAME}.dll")
	set(ZLIB_SHARED_LIB "${ZLIB_NAME}.dll")
else()
	message(FATAL_ERROR "Please fix this")
endif()

add_project(glew COMMANDS
	URL "http://downloads.sourceforge.net/project/glew/glew/2.0.0/glew-2.0.0.tgz"
	URL_MD5 2a2cd7c98f13854d2fcddae0d2b20411
	CONFIGURE_COMMAND "${CMAKE_COMMAND}" ../../src/glew/build/cmake/ -G "${CMAKE_GENERATOR}" ${EXT_CM_INSTALL_PREFIX} -DBUILD_UTILS=OFF
	BUILD_COMMAND "${CMAKE_COMMAND}" --build . --config Release
	INSTALL_COMMAND "${CMAKE_COMMAND}" --build . --config Release --target install
)

add_project(glfw COMMANDS
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.2.1"
	CMAKE_ARGS -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF
)

add_custom_command(TARGET glfw POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIB_DIR}/${GLFW_SHARED_LIB} ${BIN_DIR}/${GLFW_SHARED_LIB}
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

add_project(zlib COMMANDS
	URL "http://zlib.net/zlib-1.2.8.tar.gz"
	URL_MD5 44d667c142d7cda120332623eab69f40
	CMAKE_ARGS -DBUILD_SHARED_LIBS=ON
)

add_project(assimp COMMANDS
	GIT_REPOSITORY "https://github.com/assimp/assimp.git"
	GIT_TAG "v3.3.1"
	CMAKE_ARGS ${ZLIB_PKG_ARGS} -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_SAMPLES=OFF -DBUILD_SHARED_LIBS=ON -DLIBRARY_SUFFIX=
)

ExternalProject_Add_StepDependencies(assimp configure zlib)

add_project(tiff COMMANDS
	URL "ftp://download.osgeo.org/libtiff/tiff-4.0.7.tar.gz"
	URL_MD5 77ae928d2c6b7fb46a21c3a29325157b
	CMAKE_ARGS ${ZLIB_PKG_ARGS}
)

ExternalProject_Add_StepDependencies(tiff configure zlib)

set(EXT_LINK_LIBS
	glew32
	glfw3dll
	${TIFF_NAME}
)

set(EXT_SHARED_LIBS 
	${GLEW_SHARED_LIB}
	${GLFW_SHARED_LIB}
	${TIFF_SHARED_LIB}
	${ZLIB_SHARED_LIB}
)

include(add_boost)

list(APPEND EXT_LINK_LIBS ${BOOST_LIBS})