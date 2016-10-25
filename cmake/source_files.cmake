set(HEADER_FILES
	include/Component.hpp
	include/Engine.hpp
	include/Entity.hpp
	include/ForwardRenderEngine.hpp
	include/glm.hpp
	include/Mesh.hpp
	include/MeshRenderer.hpp
	include/RenderEngine.hpp
	include/Renderer.hpp
	include/Scene.hpp
	include/SceneImporter.hpp
	include/typedefs.hpp
)

set(SOURCE_FILES
	src/Component.cpp
	src/Engine.cpp
	src/Entity.cpp
	src/ForwardRenderEngine.cpp
	src/main.cpp
	src/Mesh.cpp
	src/MeshRenderer.cpp
	src/RenderEngine.cpp
	src/Renderer.cpp
	src/Scene.cpp
	src/SceneImporter.cpp
)

set(CMAKE_FILES
	cmake/add_boost.cmake
	cmake/external_libs.cmake
	cmake/source_files.cmake
)
source_group("CMake Files" FILES ${CMAKE_FILES})

set(SCRIPT_FILES
	scripts/find_sources.py
	scripts/msvc14.bat
)
source_group("Script Files" FILES ${SCRIPT_FILES})

set(CONTENT_FILES
)

set(ALL_SOURCE_FILES
	${HEADER_FILES}
	${SOURCE_FILES}
	${CMAKE_FILES}
	${SCRIPT_FILES}
)
