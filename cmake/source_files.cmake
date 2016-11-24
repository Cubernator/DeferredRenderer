set(HEADER_FILES
	include/glm.hpp
	include/path.hpp
	include/uuid.hpp
	include/components/FlyCamera.hpp
	include/core/Component.hpp
	include/core/Content.hpp
	include/core/Engine.hpp
	include/core/Entity.hpp
	include/core/Input.hpp
	include/core/Scene.hpp
	include/core/Transform.hpp
	include/graphics/Buffer.hpp
	include/graphics/Camera.hpp
	include/graphics/Effect.hpp
	include/graphics/gl_types.hpp
	include/graphics/Light.hpp
	include/graphics/Material.hpp
	include/graphics/Mesh.hpp
	include/graphics/MeshRenderer.hpp
	include/graphics/RenderEngine.hpp
	include/graphics/Renderer.hpp
	include/graphics/render_state.hpp
	include/graphics/shader/set_uniform.hpp
	include/graphics/shader/Shader.hpp
	include/graphics/shader/ShaderProgram.hpp
	include/graphics/shader/shader_property.hpp
	include/graphics/shader/uniform_id.hpp
	include/graphics/texture/image_importer.hpp
	include/graphics/texture/pixel_types.hpp
	include/graphics/texture/raw_img_importer.hpp
	include/graphics/texture/Sampler.hpp
	include/graphics/texture/Texture.hpp
	include/graphics/texture/Texture2D.hpp
	include/graphics/texture/texture_unit_manager.hpp
	include/util/app_info.hpp
	include/util/bounds.hpp
	include/util/component_registry.hpp
	include/util/import.hpp
	include/util/intersection_tests.hpp
	include/util/json_initializable.hpp
	include/util/json_interpreter.hpp
	include/util/json_utils.hpp
	include/util/keyword_helper.hpp
	include/util/object_pool.hpp
	include/util/property_interpreter.hpp
	include/util/shader_preprocessor.hpp
	include/util/type_registry.hpp
)

set(SOURCE_FILES
	src/main.cpp
	src/components/FlyCamera.cpp
	src/core/Component.cpp
	src/core/Content.cpp
	src/core/Engine.cpp
	src/core/Entity.cpp
	src/core/Input.cpp
	src/core/Scene.cpp
	src/core/Transform.cpp
	src/graphics/Camera.cpp
	src/graphics/Effect.cpp
	src/graphics/Light.cpp
	src/graphics/Material.cpp
	src/graphics/Mesh.cpp
	src/graphics/MeshRenderer.cpp
	src/graphics/RenderEngine.cpp
	src/graphics/Renderer.cpp
	src/graphics/render_state.cpp
	src/graphics/shader/Shader.cpp
	src/graphics/shader/ShaderProgram.cpp
	src/graphics/shader/shader_property.cpp
	src/graphics/texture/raw_img_importer.cpp
	src/graphics/texture/Texture2D.cpp
	src/graphics/texture/texture_unit_manager.cpp
	src/util/app_info.cpp
	src/util/component_registry.cpp
	src/util/intersection_tests.cpp
	src/util/json_utils.cpp
	src/util/object_pool.cpp
	src/util/shader_preprocessor.cpp
	src/util/type_registry.cpp
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
	content/appinfo.json
	content/effects/effect-diffuse.json
	content/effects/effect-pbr.json
	content/effects/effect-unlit.json
	content/materials/material-stonewall.json
	content/meshes/cube.fbx
	content/scenes/scene-test.json
	content/shaders/common/input.glh
	content/shaders/common/lighting.glh
	content/shaders/common/uniforms.glh
	content/shaders/diffuse/diffuse.frag.glsl
	content/shaders/diffuse/diffuse.vert.glsl
	content/shaders/diffuse/diffuse_common.glh
	content/shaders/pbr/pbr.frag.glsl
	content/shaders/pbr/pbr.vert.glsl
	content/shaders/pbr/pbr_brdf.glh
	content/shaders/pbr/pbr_common.glh
	content/shaders/unlit/unlit.frag.glsl
	content/shaders/unlit/unlit.vert.glsl
	content/shaders/unlit/unlit_common.glh
	content/textures/test.raw
	content/textures/testnormal.raw
	content/textures/texture-test.json
	content/textures/texture-testnormal.json
)
source_group("Content Files" FILES ${CONTENT_FILES})

set(ALL_SOURCE_FILES
	${HEADER_FILES}
	${SOURCE_FILES}
	${CMAKE_FILES}
	${SCRIPT_FILES}
	${CONTENT_FILES}
)
