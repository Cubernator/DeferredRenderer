set(BOOST_VERSION 1.64.0)
set(BOOST_COMPONENTS
	system
	filesystem
	program_options
	date_time
	thread
	regex
	log
	log_setup
)
set(BOOST_HASH 0445c22a5ef3bd69f5dfb48354978421a85ab395254a26b1ffb0aa1bfd63a108)

string(REPLACE "." "_" BOOST_FOLDER ${BOOST_VERSION})
set(BOOST_FOLDER boost_${BOOST_FOLDER})
set(BOOST_ARCHIVE_FILENAME ${BOOST_FOLDER}.tar.gz)

set(BOOST_URL "https://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION}/${BOOST_ARCHIVE_FILENAME}/download")

set(IS_DBG $<CONFIG:Debug>)
set(IS_REL $<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>)
set(IS_PRF $<CONFIG:RelWithDebInfo>)
set(VARIANT $<${IS_DBG}:debug>$<${IS_REL}:release>$<${IS_PRF}:profile>)

set(ADDR_MODEL "")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ADDR_MODEL address-model=64)
endif()

set(BUILD_CMD
	"<SOURCE_DIR>/b2"
	variant=${VARIANT}
	link=static
	threading=multi
	runtime-link=shared
	architecture=x86
	${ADDR_MODEL}
	--build-dir=${EXT_BINARY_DIR}/boost/
	--prefix=<INSTALL_DIR>
	--layout=tagged
	install
)

set(LIB_PREFIX "")

if(MSVC)
	if(MSVC_VERSION EQUAL 1700)
		set(TOOLSET msvc-11.0)
	elseif(MSVC_VERSION EQUAL 1800)
		set(TOOLSET msvc-12.0)
	elseif(MSVC_VERSION EQUAL 1900)
		set(TOOLSET msvc-14.0)
	elseif(MSVC_VERSION EQUAL 1910)
		set(TOOLSET msvc-14.1)
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
	#HACK: boost.log produces two libraries... WHY???
	if(NOT ${component} STREQUAL log_setup)
		list(APPEND BUILD_CMD --with-${component})
	endif()
	
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
	URL_HASH SHA256=${BOOST_HASH}
		
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
