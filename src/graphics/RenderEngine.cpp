#include "graphics/RenderEngine.hpp"

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

uniform_id RenderEngine::s_cm_mat_proj_id = uniform_name_to_id("cm_mat_proj");
uniform_id RenderEngine::s_cm_mat_view_id = uniform_name_to_id("cm_mat_view");
uniform_id RenderEngine::s_cm_mat_world_id = uniform_name_to_id("cm_mat_world");

uniform_id RenderEngine::s_cm_mat_tiworld_id = uniform_name_to_id("cm_mat_tiworld");
uniform_id RenderEngine::s_cm_mat_vp_id = uniform_name_to_id("cm_mat_vp");
uniform_id RenderEngine::s_cm_mat_wvp_id = uniform_name_to_id("cm_mat_wvp");

uniform_id RenderEngine::s_cm_cam_pos_id = uniform_name_to_id("cm_cam_pos");

uniform_id RenderEngine::s_cm_light_color_id = uniform_name_to_id("cm_light_color");
uniform_id RenderEngine::s_cm_light_dir_id = uniform_name_to_id("cm_light_dir");
uniform_id RenderEngine::s_cm_light_radius_id = uniform_name_to_id("cm_light_radius");

RenderEngine::RenderEngine(Engine* parent) : m_parent(parent), m_maxLights(8)
{
	m_lightQueue.reserve(m_maxLights);

	m_renderState.apply();

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void RenderEngine::render()
{
	glm::vec4 bc(0.f);

	Scene* scene = m_parent->getScene();
	if (scene) {
		bc = scene->getBackColor();
	}

	glDepthMask(GL_TRUE);
	glClearColor(bc.r, bc.g, bc.b, bc.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Camera* mainCam = Camera::main();
	if (!mainCam) return; // can't render anything without a camera!

	int sw, sh;
	m_parent->getScreenSize(sw, sh);

	m_objUniforms.setProjView(mainCam, float(sw), float(sh));

	m_deferredQueue.clear();
	m_forwardQueue.clear();

	m_deferredQueue.reserve(m_renderers.size());
	m_forwardQueue.reserve(m_renderers.size());

	// TODO: implement render queues and render types

	// sort objects into render queues
	for (Renderer* renderer : m_renderers) {
		if (!(renderer->isActiveAndEnabled() && renderer->isVisible()))
			continue;

		Material* material = renderer->getMaterial();
		if (!material)
			continue;

		Effect* effect = material->getEffect();
		if (!effect)
			continue;

		// TODO: view frustum culling

		// if the object is not transparent and has a deferred pass, put it into deferred queue (deferred pass will be ignored in transparent effects)
		if (effect->getRenderType() != Effect::type_transparent) {
			const Effect::pass* passDeferred = effect->getPass(Effect::light_deferred);
			if (passDeferred && passDeferred->program) {
				m_deferredQueue.emplace_back(renderer, passDeferred);
				continue;
			}
		}

		// if not, check if forward pass is present
		const Effect::pass* passFwdBase = effect->getPass(Effect::light_forward_base);
		if (passFwdBase && passFwdBase->program) {
			m_forwardQueue.emplace_back(renderer, passFwdBase);
			continue;
		}

		// skip it otherwise (something probably went wrong while initializing this object)
	}

	// render all objects that support deferred shading
	//renderDeferred();

	// the rest is rendered using good ol' forward rendering
	renderForward();
}

void RenderEngine::renderDeferred()
{
	// G-Buffer pass
	for (const auto& obj : m_deferredQueue) {

	}

	// Lighting pass
}

void RenderEngine::renderForward()
{
	// sort lights by type
	for (Light* light : m_lights) {
		if (!light->isActiveAndEnabled()) continue;

		if (light->getType() == Light::type_directional) {
			m_dirLights.insert(light);
		} else {
			m_posLights.insert(light);
		}
	}

	// render objects
	for (const auto& obj : m_forwardQueue) {
		m_lightQueue.clear();

		// first try applying all directional lights
		for (Light* light : m_dirLights) {
			if (m_lightQueue.size() >= m_maxLights) break;
			m_lightQueue.push_back(light);
		}

		// then all intersecting non-directional lights
		for (Light* light : m_posLights) {
			if (m_lightQueue.size() >= m_maxLights) break;
			if (light->checkIntersection(obj.renderer)) // only if object intersects light's sphere of influence
				m_lightQueue.push_back(light);
		}

		// if no lights are affecting this object, draw at least once with no light so it won't be invisible
		if (m_lightQueue.empty()) m_lightQueue.push_back(nullptr);

		m_objUniforms.setWorld(obj.renderer->getEntity());

		Material* material = obj.renderer->getMaterial();
		auto lit = m_lightQueue.begin();

		// render first light with base pass
		bindPass(obj.pass, material);
		updateRenderState(obj.pass->state);
		applyLight(*lit, obj.pass->program);
		obj.renderer->bind();
		obj.renderer->draw();
		++lit;

		// then render all remaining lights with additive pass (different depth test, no depth writing and additive blending)
		const Effect::pass* passFwdAdd = material->getEffect()->getPass(Effect::light_forward_add);
		if (passFwdAdd && passFwdAdd->program) {
			if (passFwdAdd->program != obj.pass->program) // no need to re-bind shader and re-apply uniforms if program is the same
				bindPass(passFwdAdd, material);
			updateRenderState(passFwdAdd->state);

			while (lit != m_lightQueue.end()) {
				applyLight(*lit, passFwdAdd->program);
				obj.renderer->draw();
				++lit;
			}
		}

		obj.renderer->unbind();
	}

	m_dirLights.clear();
	m_posLights.clear();
}

void RenderEngine::addEntity(Entity* entity)
{
	Renderer* r = entity->getComponent<Renderer>();
	if (r) {
		m_renderers.push_back(r);
	}

	Light* l = entity->getComponent<Light>();
	if (l) {
		m_lights.push_back(l);
	}
}

void RenderEngine::removeEntity(Entity* entity)
{
	Renderer* r = entity->getComponent<Renderer>();
	if (r) {
		m_renderers.erase(std::remove(m_renderers.begin(), m_renderers.end(), r), m_renderers.end());
	}

	Light* l = entity->getComponent<Light>();
	if (l) {
		m_lights.erase(std::remove(m_lights.begin(), m_lights.end(), l), m_lights.end());
	}
}

void RenderEngine::bindPass(const Effect::pass* pass, Material* material)
{
	pass->program->bind();
	m_objUniforms.apply(pass->program);
	material->getEffect()->applyProperties(pass, material);
}

void RenderEngine::applyLight(Light* light, ShaderProgram* program)
{
	if (light) {
		program->setUniform(s_cm_light_color_id, light->getPremultipliedColor());
		program->setUniform(s_cm_light_dir_id, light->getShaderProp());
		program->setUniform(s_cm_light_radius_id, light->getRange());
	}
}

void RenderEngine::updateRenderState(const render_state& newState)
{
	newState.differentialApply(m_renderState);
	m_renderState = newState;
}

bool RenderEngine::light_priority_comparer::operator()(const Light* lhs, const Light* rhs) const
{
	return lhs->getPriority() > rhs->getPriority();
}

void RenderEngine::uniforms_per_obj::setProjView(Camera* camera, float w, float h)
{
	Transform* camTrans = camera->getEntity()->getTransform();
	proj = camera->getProjectionMatrix(w, h);
	view = camTrans->getInverseRigidMatrix();
	vp = proj * view;
	camPos = camTrans->getPosition();
}

void RenderEngine::uniforms_per_obj::setWorld(Entity* entity)
{
	world = entity->getTransform()->getMatrix();
	tiworld = glm::transpose(glm::inverse(world));
	wvp = vp * world;
}

void RenderEngine::uniforms_per_obj::apply(ShaderProgram* program) const
{
	program->setUniform(s_cm_mat_proj_id, proj);
	program->setUniform(s_cm_mat_view_id, view);
	program->setUniform(s_cm_mat_world_id, world);
	program->setUniform(s_cm_mat_tiworld_id, tiworld);
	program->setUniform(s_cm_mat_vp_id, vp);
	program->setUniform(s_cm_mat_wvp_id, wvp);
	program->setUniform(s_cm_cam_pos_id, camPos);
}
