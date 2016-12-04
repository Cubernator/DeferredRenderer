#include "graphics/RenderEngine.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "core/Content.hpp"
#include "graphics/Material.hpp"
#include "graphics/shader/ShaderProgram.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Light.hpp"
#include "graphics/texture/Texture2D.hpp"
#include "util/app_info.hpp"

#include "GL/glew.h"

#include <vector>

#define DEF_UNIFORM_ID(name) const uniform_id g_##name##_id = uniform_name_to_id(#name)

// Basic matrices
DEF_UNIFORM_ID(cm_mat_proj);
DEF_UNIFORM_ID(cm_mat_view);
DEF_UNIFORM_ID(cm_mat_world);

// Combined matrices
DEF_UNIFORM_ID(cm_mat_tiworld);
DEF_UNIFORM_ID(cm_mat_vp);
DEF_UNIFORM_ID(cm_mat_ivp);
DEF_UNIFORM_ID(cm_mat_wvp);

// Vectors
DEF_UNIFORM_ID(cm_cam_pos);

// Light parameters
DEF_UNIFORM_ID(cm_light_ambient);
DEF_UNIFORM_ID(cm_light_color);
DEF_UNIFORM_ID(cm_light_dir);
DEF_UNIFORM_ID(cm_light_spot);
DEF_UNIFORM_ID(cm_light_atten);

// Deferred Light parameters
DEF_UNIFORM_ID(transform);
DEF_UNIFORM_ID(gbuf_diffuse);
DEF_UNIFORM_ID(gbuf_specSmooth);
DEF_UNIFORM_ID(gbuf_normal);
DEF_UNIFORM_ID(gbuf_depth);

RenderEngine* RenderEngine::s_instance = nullptr;

RenderEngine::RenderEngine(Engine* parent)
	: m_parent(parent), m_maxLights(8), m_gBufFBO(0), m_lightMeshVAO(0),
	m_deferredAmbientPass(nullptr), m_deferredLightPass(nullptr),
	m_enableDeferred(true)
{
	s_instance = this;

	m_lightQueue.reserve(m_maxLights);
	setupDeferredPath();
	m_renderState.apply();
	glEnable(GL_FRAMEBUFFER_SRGB);
}

void RenderEngine::setupDeferredPath()
{
	m_gBufDiff = std::make_unique<Texture2D>();
	m_gBufDiff->setParams(false, Texture2D::filter_point, Texture2D::wrap_clampToEdge);

	m_gBufSpec = std::make_unique<Texture2D>();
	m_gBufSpec->setParams(false, Texture2D::filter_point, Texture2D::wrap_clampToEdge);

	m_gBufNorm = std::make_unique<Texture2D>();
	m_gBufNorm->setParams(false, Texture2D::filter_point, Texture2D::wrap_clampToEdge);

	m_gBufDepth = std::make_unique<Texture2D>();
	m_gBufDepth->setParams(false, Texture2D::filter_point, Texture2D::wrap_clampToEdge);

	onResize(m_parent->getScreenWidth(), m_parent->getScreenHeight());

	glGenFramebuffers(1, &m_gBufFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBufDiff->glObj(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBufSpec->glObj(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gBufNorm->glObj(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gBufDepth->glObj(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Deferred framebuffer object is incomplete!" << std::endl;
	}

	m_gDrawBufs[0] = GL_COLOR_ATTACHMENT0;
	m_gDrawBufs[1] = GL_COLOR_ATTACHMENT1;
	m_gDrawBufs[2] = GL_COLOR_ATTACHMENT2;

	m_gClearColor[0] = 0.0f;
	m_gClearColor[1] = 0.0f;
	m_gClearColor[2] = 0.0f;
	m_gClearColor[3] = 0.0f;

	m_gClearDepth = 1.0f;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	createCombinedLightMesh();

	auto lightEffectName = app_info::get<std::string>("deferredLightEffect", "deferred_light");
	m_deferredLightEffect = Content::instance()->getFromDisk<Effect>(lightEffectName);

	if (m_deferredLightEffect) {
		m_deferredAmbientPass = m_deferredLightEffect->getPass(Effect::light_deferred_ambient);
		m_deferredLightPass = m_deferredLightEffect->getPass(Effect::light_deferred_light);

		if (!m_deferredAmbientPass) {
			std::cout << "WARNING: could not initialize deferred ambient pass!" << std::endl;
		}

		if (!m_deferredLightPass) {
			std::cout << "WARNING: could not initialize deferred light pass!" << std::endl;
		}
	} else {
		std::cout << "WARNING: could not initialize deferred light effect!" << std::endl;
	}
}

void RenderEngine::createCombinedLightMesh()
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::size_t baseIndex;
	unsigned int b;


	// generate quad
	baseIndex = indices.size();
	b = vertices.size();
	vertices.insert(vertices.end(), {
		{ -1.f, -1.f, 0.f },
		{ 1.f, -1.f, 0.f },
		{ -1.f, 1.f, 0.f },
		{ 1.f, 1.f, 0.f }
	});
	indices.insert(indices.end(), {
		b+0, b+1, b+2,
		b+2, b+1, b+3
	});
	m_quadOffset = sizeof(unsigned int) * baseIndex;
	m_quadCount = indices.size() - baseIndex;


	// generate sphere
	baseIndex = indices.size();
	b = vertices.size();

	const unsigned int rings = 6;
	const unsigned int sectors = rings * 2;
	const float alpha = glm::pi<float>() / sectors;
	// adjust radius so the mesh encompasses the sphere
	// NOTE: may be wrong still
	const float r = 1.0f / cosf(alpha);
	m_lightMeshRadius = r;

	unsigned int rb = b + 1;
	vertices.push_back({ 0.f, r, 0.f });
	for (unsigned int si = 0; si < sectors; ++si) {
		indices.insert(indices.end(), {
			b, rb + si, rb + ((si < (sectors - 1)) ? (si + 1) : 0)
		});
	}

	for (unsigned int ri = 1; ri < rings; ++ri) {
		float theta = (glm::pi<float>() * ri) / rings;
		float sint = sinf(theta), cost = cosf(theta);

		for (unsigned int si = 0; si < sectors; ++si) {
			float phi = (glm::two_pi<float>() * si) / sectors;
			vertices.push_back({
				r * sint * sinf(phi),
				r * cost,
				r * sint * cosf(phi)
			});

			if (ri < (rings - 1)) {
				unsigned int sn = (si < (sectors - 1)) ? (si + 1) : 0;

				indices.insert(indices.end(), {
					rb + si,
					rb + si + sectors,
					rb + sn,
					rb + sn,
					rb + si + sectors,
					rb + sn + sectors,
				});
			}
		}

		rb += sectors;
	}
	b = vertices.size();
	rb -= sectors;
	vertices.push_back({ 0.f, -r, 0.f });
	for (unsigned int si = 0; si < sectors; ++si) {
		indices.insert(indices.end(), {
			b, rb + ((si < (sectors - 1)) ? (si + 1) : 0), rb + si
		});
	}

	m_sphereOffset = sizeof(unsigned int) * baseIndex;
	m_sphereCount = indices.size() - baseIndex;

	m_lightMeshVbuf.setData(vertices.size(), vertices.data());
	m_lightMeshIbuf.setData(indices.size(), indices.data());

	glGenVertexArrays(1, &m_lightMeshVAO);
	glBindVertexArray(m_lightMeshVAO);
	glEnableVertexAttribArray(0);
	m_lightMeshVbuf.bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
}

RenderEngine::~RenderEngine()
{
	if (m_gBufFBO) {
		glDeleteFramebuffers(1, &m_gBufFBO);
	}

	if (m_lightMeshVAO) {
		glDeleteVertexArrays(1, &m_lightMeshVAO);
	}
}

void RenderEngine::onResize(int width, int height)
{
	glViewport(0, 0, width, height);

	m_gBufDiff->setData<pixel::rgb32f>(nullptr, width, height);
	m_gBufSpec->setData<pixel::rgba32f>(nullptr, width, height);
	m_gBufNorm->setData<pixel::rgb10a2>(nullptr, width, height);
	m_gBufDepth->setData<pixel::depth32>(nullptr, width, height);
}

void RenderEngine::render()
{
	Scene* scene = m_parent->getScene();
	if (scene) {
		m_clearColor = scene->getBackColor();
		m_ambientLight = scene->getAmbientLight();
	}

	glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_camera = Camera::main();
	if (!m_camera) return; // can't render anything without a camera!

	int sw, sh;
	m_parent->getScreenSize(sw, sh);

	m_objUniforms.setProjView(m_camera, float(sw), float(sh));

	m_dirLights.clear();
	m_posLights.clear();

	// sort lights by type
	for (Light* light : m_lights) {
		if (!light->isActiveAndEnabled()) continue;

		if (light->getType() == Light::type_directional) {
			m_dirLights.insert(light);
		} else {
			m_posLights.insert(light);
		}
	}

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
		if (m_enableDeferred && (effect->getRenderType() != Effect::type_transparent)) {
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
	renderDeferred();

	// the rest is rendered using good ol' forward rendering
	renderForward();
}

void RenderEngine::renderDeferred()
{
	if (m_deferredQueue.empty() || !(m_deferredAmbientPass && m_deferredAmbientPass->program)) return;

	// G-Buffer pass
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufFBO);
	glDrawBuffers(3, m_gDrawBufs);

	glClearBufferfv(GL_COLOR, 0, m_gClearColor);
	glClearBufferfv(GL_COLOR, 1, m_gClearColor);
	glClearBufferfv(GL_COLOR, 2, m_gClearColor);
	glClearBufferfv(GL_DEPTH, 0, &m_gClearDepth);

	for (const auto& obj : m_deferredQueue) {
		m_objUniforms.setWorld(obj.renderer->getEntity());
		Material* material = obj.renderer->getMaterial();
		bindPass(obj.pass, material);
		updateRenderState(obj.pass->state);
		obj.renderer->bind();
		obj.renderer->draw();
		obj.renderer->unbind();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Lighting pass
	glBindVertexArray(m_lightMeshVAO);
	m_lightMeshIbuf.bind();

	// Render ambient light
	bindDeferredLightPass(m_deferredAmbientPass);
	applyAmbient(true, m_deferredAmbientPass->program);
	drawDeferredLight(nullptr, m_deferredAmbientPass->program);

	if (!(m_deferredLightPass && m_deferredLightPass->program)) return;

	// Render normal lights
	bindDeferredLightPass(m_deferredLightPass);
	applyAmbient(false, m_deferredLightPass->program);

	// First directional lights, then positional lights
	m_lightQueue.assign(m_dirLights.begin(), m_dirLights.end());
	m_lightQueue.insert(m_lightQueue.end(), m_posLights.begin(), m_posLights.end());

	for (Light* light : m_lightQueue) {
		applyLight(light, m_deferredLightPass->program);
		drawDeferredLight(light, m_deferredLightPass->program);
	}
}

void RenderEngine::renderForward()
{
	if (m_forwardQueue.empty()) return;

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
		applyAmbient(true, obj.pass->program);
		obj.renderer->bind();
		obj.renderer->draw();
		++lit;

		// then render all remaining lights with additive pass (different depth test, no depth writing and additive blending)
		const Effect::pass* passFwdAdd = material->getEffect()->getPass(Effect::light_forward_add);
		if (passFwdAdd && passFwdAdd->program) {
			if (passFwdAdd->program != obj.pass->program) // no need to re-bind shader and re-apply uniforms if program is the same
				bindPass(passFwdAdd, material);

			// disable ambient light in additive pass
			applyAmbient(false, passFwdAdd->program);
			updateRenderState(passFwdAdd->state);

			while (lit != m_lightQueue.end()) {
				applyLight(*lit, passFwdAdd->program);
				obj.renderer->draw();
				++lit;
			}
		}

		obj.renderer->unbind();
	}
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

void RenderEngine::bindDeferredLightPass(const Effect::pass * pass)
{
	updateRenderState(pass->state);

	ShaderProgram* program = pass->program;
	program->bind();
	program->setUniform(g_cm_mat_ivp_id, m_objUniforms.ivp);
	program->setUniform(g_cm_cam_pos_id, m_objUniforms.camPos);
	program->setTexture(g_gbuf_diffuse_id, m_gBufDiff.get());
	program->setTexture(g_gbuf_specSmooth_id, m_gBufSpec.get());
	program->setTexture(g_gbuf_normal_id, m_gBufNorm.get());
	program->setTexture(g_gbuf_depth_id, m_gBufDepth.get());
}

void RenderEngine::drawDeferredLight(Light* light, ShaderProgram* program)
{
	glm::mat4 transform;
	auto o = m_quadOffset;
	auto c = m_quadCount;

	if (light) {
		Light::type t = light->getType();

		if (t != Light::type_directional) {
			float r = light->getRange();
			float lr = r * m_lightMeshRadius;
			Transform* lt = light->getEntity()->getTransform();
			glm::vec4 lp = {lt->getPosition(), 1.0f};
			glm::vec3 lpos = m_objUniforms.view * lp;
			
			// TODO: improve this test and add frustum culling
			if ((-lpos.z - lr) > m_camera->getNearPlane()) {
				transform = m_objUniforms.vp * lt->getRigidMatrix() * glm::scale(glm::vec3(r));
				// TODO: add spot light mesh
				switch (t) {
				case Light::type_point:
				case Light::type_spot:
					c = m_sphereCount;
					o = m_sphereOffset;
					break;
				}
			}
		}
	}

	program->setUniform(g_transform_id, transform);
	glDrawElements(GL_TRIANGLES, c, GL_UNSIGNED_INT, reinterpret_cast<void*>(o));
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
		glm::vec4 color, dir, atten, spot;
		light->getUniforms(color, dir, atten, spot);
		program->setUniform(g_cm_light_color_id, color);
		program->setUniform(g_cm_light_dir_id, dir);
		program->setUniform(g_cm_light_atten_id, atten);
		program->setUniform(g_cm_light_spot_id, spot);
	}
}

void RenderEngine::applyAmbient(bool enabled, ShaderProgram* program)
{
	program->setUniform(g_cm_light_ambient_id, enabled ? m_ambientLight : glm::vec4(0.f));
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
	ivp = glm::inverse(vp);
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
	program->setUniform(g_cm_mat_proj_id, proj);
	program->setUniform(g_cm_mat_view_id, view);
	program->setUniform(g_cm_mat_world_id, world);
	program->setUniform(g_cm_mat_tiworld_id, tiworld);
	program->setUniform(g_cm_mat_vp_id, vp);
	program->setUniform(g_cm_mat_ivp_id, ivp);
	program->setUniform(g_cm_mat_wvp_id, wvp);
	program->setUniform(g_cm_cam_pos_id, camPos);
}
