project(conproc CXX C)

include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

set(HEADER_FILES
	conproc.hpp
	processor.hpp
	generic.hpp
	ignore.hpp
	mesh.hpp
	image.hpp
)

set(SOURCE_FILES
	main.cpp
	conproc.cpp
	processor.cpp
	generic.cpp
	mesh.cpp
	image.cpp
)

set(CP_LINK_LIBS assimp)

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

add_executable(conproc ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(conproc PROPERTIES OUTPUT_NAME_DEBUG conprocd)
set_target_out_dir(conproc ${BIN_DIR})

target_link_libraries(conproc ${CP_LINK_LIBS})

set(CONPROC_COMMAND ${BIN_DIR}/${CP_OUTPUT_NAME} PARENT_SCOPE)
