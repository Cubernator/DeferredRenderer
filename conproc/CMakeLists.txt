project(conproc CXX C)

include_directories(${INCLUDE_DIR} ../contrib ../util)
link_directories(${LIB_DIR})

set(SOURCE_FILES
	conproc.hpp
	conproc.cpp
	generic.hpp
	generic.cpp
	ignore.hpp
	image.hpp
	image.cpp
	main.cpp
	mesh.hpp
	mesh.cpp
	processor.hpp
	processor.cpp
)

set(TIFF_NAME tiff${DBG_PREFIX})
set(SQUISH_NAME squish${DBG_PREFIX})

set(CP_LINK_LIBS
	assimp
	${TIFF_NAME}
	${SQUISH_NAME}
)

set(LIB_PREFIX "")
if(MSVC)
	set(LIB_PREFIX "lib")
endif()

set(BOOST_CMPS
	system
	filesystem
	program_options
)

foreach(component ${BOOST_CMPS})
	list(APPEND CP_LINK_LIBS ${LIB_PREFIX}boost_${component}-mt$<$<CONFIG:Debug>:-gd>)
endforeach()

set(CP_OUTPUT_NAME conproc${DBG_PREFIX})

add_executable(conproc ${SOURCE_FILES})
set_target_properties(conproc PROPERTIES OUTPUT_NAME_DEBUG conprocd)
set_target_out_dir(conproc ${BIN_DIR})

target_link_libraries(conproc ${CP_LINK_LIBS})

set(CONPROC_COMMAND ${BIN_DIR}/${CP_OUTPUT_NAME} PARENT_SCOPE)
