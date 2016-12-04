set(BOOST_VERSION 1.62.0)
set(BOOST_COMPONENTS
	system
	filesystem
	program_options
)

string(REPLACE "." "_" BOOST_FOLDER ${BOOST_VERSION})
set(BOOST_FOLDER boost_${BOOST_FOLDER})
set(BOOST_ARCHIVE_FILENAME ${BOOST_FOLDER}.tar.gz)

set(BOOST_URL "https://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION}/${BOOST_ARCHIVE_FILENAME}/download")

set(BUILD_CMD
	"<SOURCE_DIR>/b2"
	variant=$<LOWER_CASE:$<CONFIG>>
	link=static
	threading=multi
	runtime-link=shared
	--build-dir=${EXT_BINARY_DIR}/boost/
	--prefix=<INSTALL_DIR>
	--layout=tagged
	install
)

set(LIB_PREFIX "")

if(MSVC)
	if(MSVC11)
		set(TOOLSET msvc-11.0)
	elseif(MSVC12)
		set(TOOLSET msvc-12.0)
	elseif(MSVC14)
		set(TOOLSET msvc-14.0)
	endif()
	set(LIB_PREFIX "lib")
elseif(MINGW)
	set(TOOLSET mingw)
else()
	set(TOOLSET gcc)
endif()
list(APPEND BUILD_CMD toolset=${TOOLSET})

set(BOOST_LIBS "")

foreach(component ${BOOST_COMPONENTS})
	list(APPEND BUILD_CMD --with-${component})
	list(APPEND BOOST_LIBS ${LIB_PREFIX}boost_${component}-mt$<$<CONFIG:Debug>:-gd>)
endforeach()

ExternalProject_Add(
	boost
	TMP_DIR			${EXT_TMP_DIR}
	STAMP_DIR		${EXT_STAMP_DIR}
	DOWNLOAD_DIR	${EXT_DOWNLOAD_DIR}
	SOURCE_DIR		${EXT_SOURCE_DIR}/boost
	BINARY_DIR		${EXT_SOURCE_DIR}/boost
	INSTALL_DIR		${EXT_INSTALL_DIR}
	
	URL ${BOOST_URL}
		
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ${BUILD_CMD}
	INSTALL_COMMAND ""
)

if(MSVC)
	set(BOOTSTRAP_SCRIPT "bootstrap.bat")
else()
	set(BOOTSTRAP_SCRIPT "bootstrap.sh")
endif()

ExternalProject_Add_Step(
	boost bootstrap
	DEPENDEES configure
	DEPENDERS build
	COMMAND ${BOOTSTRAP_SCRIPT}
	WORKING_DIRECTORY <SOURCE_DIR>
	BYPRODUCTS "<SOURCE_DIR>/b2.exe"
	COMMENT "Running bootstrap script..."
)

add_dependencies(DeferredRenderer boost)
target_link_libraries(DeferredRenderer ${BOOST_LIBS})

add_definitions( -DBOOST_ALL_NO_LIB )
