#include "graphics/ForwardRenderEngine.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "graphics/Material.hpp"
#include "graphics/Effect.hpp"
#include "graphics/ShaderProgram.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Camera.hpp"

#include "GL/glew.h"

void ForwardRenderEngine::doRender()
{
	Engine* parent = getParent();

	glm::vec4 bc(0.f);

	Scene* scene = parent->getScene();
	if (scene) {
		bc = scene->getBackColor();
	}

	glClearColor(bc.r, bc.g, bc.b, bc.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Camera* mainCam = Camera::main();
	if (!mainCam) return; // can't render anything without a camera!

	int sw, sh;
	parent->getScreenSize(sw, sh);
	glm::mat4 proj = mainCam->getProjectionMatrix(float(sw), float(sh));

	glm::mat4 view = mainCam->getEntity()->getTransform()->getInverseRigidMatrix();

	// TODO: implement render queues and render types

	// TODO: implement lighting

	for (auto it = parent->entities_begin(); it != parent->entities_end(); ++it) {
		auto& entity = it->second;
		Renderer* renderer = entity->getComponent<Renderer>();
		if (renderer && renderer->isVisible()) {
			Material* material = renderer->getMaterial();
			if (material) {
				Effect* effect = material->getEffect();
				if (effect) {
					const Effect::pass* pass = effect->getPass(Effect::light_forward);
					if (pass && pass->program) {
						glm::mat4 world = entity->getTransform()->getMatrix();
						glm::mat4 wvp = proj * view * world;

						pass->state.apply();
						pass->program->bind();

						pass->program->setUniform("common_mat_proj", proj);
						pass->program->setUniform("common_mat_view", view);
						pass->program->setUniform("common_mat_world", world);
						pass->program->setUniform("common_mat_wvp", wvp);

						effect->applyProperties(pass, material);
						renderer->render();
					}
				}
			}
		}
	}
}
