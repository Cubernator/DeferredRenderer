#include "graphics/ForwardRenderEngine.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "graphics/Material.hpp"
#include "graphics/shader/ShaderProgram.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Light.hpp"

#include "GL/glew.h"

#include <vector>

uniform_id ForwardRenderEngine::s_cm_mat_proj_id		= uniform_name_to_id("cm_mat_proj");
uniform_id ForwardRenderEngine::s_cm_mat_view_id		= uniform_name_to_id("cm_mat_view");
uniform_id ForwardRenderEngine::s_cm_mat_world_id		= uniform_name_to_id("cm_mat_world");
uniform_id ForwardRenderEngine::s_cm_mat_vp_id			= uniform_name_to_id("cm_mat_vp");
uniform_id ForwardRenderEngine::s_cm_mat_wvp_id			= uniform_name_to_id("cm_mat_wvp");

uniform_id ForwardRenderEngine::s_cm_light_color_id		= uniform_name_to_id("cm_light_color");
uniform_id ForwardRenderEngine::s_cm_light_dir_id		= uniform_name_to_id("cm_light_dir");
uniform_id ForwardRenderEngine::s_cm_light_radius_id	= uniform_name_to_id("cm_light_radius");

ForwardRenderEngine::ForwardRenderEngine(Engine* parent) : RenderEngine(parent), m_maxLights(8)
{
	m_lightQueue.reserve(m_maxLights);

	m_renderState.apply();
}

void ForwardRenderEngine::onRender()
{
	Engine* parent = getParent();

	glm::vec4 bc(0.f);

	Scene* scene = parent->getScene();
	if (scene) {
		bc = scene->getBackColor();
	}

	glDepthMask(GL_TRUE);
	glClearColor(bc.r, bc.g, bc.b, bc.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_FRAMEBUFFER_SRGB);

	Camera* mainCam = Camera::main();
	if (!mainCam) return; // can't render anything without a camera!

	int sw, sh;
	parent->getScreenSize(sw, sh);

	m_objUniforms.setProjView(mainCam, float(sw), float(sh));

	// TODO: implement render queues and render types

	for (Light* light : m_lights) {
		if (light->getType() == Light::type_directional) {
			m_dirLights.insert(light);
		} else {
			m_posLights.insert(light);
		}
	}

	for (auto it = parent->entities_begin(); it != parent->entities_end(); ++it) {
		Entity* entity = it->second.get();

		Renderer* renderer = entity->getComponent<Renderer>();
		if (!(renderer && renderer->isVisible()))
			continue;

		Material* material = renderer->getMaterial();
		if (!material)
			continue;

		Effect* effect = material->getEffect();
		if (!effect)
			continue;

		// TODO: view frustum culling

		m_lightQueue.clear();

		for (Light* light : m_dirLights) {
			if (m_lightQueue.size() >= m_maxLights) break;
			m_lightQueue.push_back(light);
		}

		for (Light* light : m_posLights) {
			if (m_lightQueue.size() >= m_maxLights) break;
			if (light->checkIntersection(renderer)) // only if object intersects light's sphere of influence
				m_lightQueue.push_back(light);
		}

		// if no lights are affecting this object, draw at least once with no light so it won't be invisible
		if (m_lightQueue.empty()) m_lightQueue.push_back(nullptr);

		const Effect::pass* passFwdBase = effect->getPass(Effect::light_forward_base);
		if (passFwdBase && passFwdBase->program) {
			m_objUniforms.setWorld(entity);

			bindPass(passFwdBase, material);
			updateRenderState(passFwdBase->state);

			auto lit = m_lightQueue.begin();
			applyLight(*lit, passFwdBase->program);
			renderer->bind();
			renderer->draw();
			++lit;

			const Effect::pass* passFwdAdd = effect->getPass(Effect::light_forward_add);
			if (passFwdAdd && passFwdAdd->program) {
				if (passFwdAdd->program != passFwdBase->program) // no need to re-bind shader and re-apply uniforms if program is the same
					bindPass(passFwdAdd, material);
				updateRenderState(passFwdAdd->state);

				while (lit != m_lightQueue.end()) {
					applyLight(*lit, passFwdBase->program);
					renderer->draw();
					++lit;
				}
			}

			renderer->unbind();
		}
	}

	m_dirLights.clear();
	m_posLights.clear();
}

void ForwardRenderEngine::onAddEntity(Entity* entity)
{
	Light* l = entity->getComponent<Light>();
	if (l) {
		m_lights.insert(l);
	}
}

void ForwardRenderEngine::onRemoveEntity(Entity* entity)
{
	Light* l = entity->getComponent<Light>();
	if (l) {
		m_lights.erase(l);
	}
}

void ForwardRenderEngine::bindPass(const Effect::pass* pass, Material* material)
{
	pass->program->bind();
	m_objUniforms.apply(pass->program);
	material->getEffect()->applyProperties(pass, material);
}

void ForwardRenderEngine::applyLight(Light* light, ShaderProgram* program)
{
	if (light) {
		program->setUniform(s_cm_light_color_id, light->getPremultipliedColor());
		program->setUniform(s_cm_light_dir_id, light->getShaderProp());
		program->setUniform(s_cm_light_radius_id, light->getRange());
	}
}

void ForwardRenderEngine::updateRenderState(const render_state& newState)
{
	newState.differentialApply(m_renderState);
	m_renderState = newState;
}

bool ForwardRenderEngine::light_priority_comparer::operator()(const Light* lhs, const Light* rhs) const
{
	return lhs->getPriority() > rhs->getPriority();
}

void ForwardRenderEngine::uniforms_per_obj::setProjView(Camera* camera, float w, float h)
{
	proj = camera->getProjectionMatrix(w, h);
	view = camera->getEntity()->getTransform()->getInverseRigidMatrix();
	vp = proj * view;
}

void ForwardRenderEngine::uniforms_per_obj::setWorld(Entity* entity)
{
	world = entity->getTransform()->getMatrix();
	wvp = vp * world;
}

void ForwardRenderEngine::uniforms_per_obj::apply(ShaderProgram* program) const
{
	program->setUniform(s_cm_mat_proj_id, proj);
	program->setUniform(s_cm_mat_view_id, view);
	program->setUniform(s_cm_mat_world_id, world);
	program->setUniform(s_cm_mat_vp_id, vp);
	program->setUniform(s_cm_mat_wvp_id, wvp);
}
