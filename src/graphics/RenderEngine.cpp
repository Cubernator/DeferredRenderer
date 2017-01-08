#include "graphics/RenderEngine.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "core/Content.hpp"
#include "graphics/Material.hpp"
#include "graphics/Effect.hpp"
#include "graphics/shader/ShaderProgram.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Light.hpp"
#include "graphics/texture/Texture2D.hpp"
#include "util/app_info.hpp"
#include "util/intersection_tests.hpp"

#include "boost/format.hpp"
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
DEF_UNIFORM_ID(mode);

RenderEngine* RenderEngine::s_instance = nullptr;

RenderEngine::RenderEngine(Engine* parent)
	: m_parent(parent), m_maxFwdLights(8), m_gBufFBO(0), m_lightMeshVAO(0),
	m_deferredAmbientPass(nullptr), m_deferredLightPass(nullptr),
	m_enableDeferred(true), m_enableViewFrustumCulling(true),
	m_outputMode(output_default)
{
	s_instance = this;

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&glDbgMsg, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

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

	GLenum drawBuffers[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};
	glDrawBuffers(3, drawBuffers);

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
		m_deferredAmbientPass = m_deferredLightEffect->getPass("ambient");
		m_deferredLightPass = m_deferredLightEffect->getPass("light");
		m_deferredDebugPass = m_deferredLightEffect->getPass("debug");

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
		{ -1.f, -1.f, -1.f },
		{ 1.f, -1.f, -1.f },
		{ -1.f, 1.f, -1.f },
		{ 1.f, 1.f, -1.f }
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
	m_width = width;
	m_height = height;

	glViewport(0, 0, m_width, m_height);

	m_gBufDiff->setData<pixel::rgb32f>(nullptr, m_width, m_height);
	m_gBufSpec->setData<pixel::rgba32f>(nullptr, m_width, m_height);
	m_gBufNorm->setData<pixel::rgb10a2>(nullptr, m_width, m_height);
	m_gBufDepth->setData<pixel::depth32>(nullptr, m_width, m_height);
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

	m_objUniforms.setPerFrame(m_camera, float(m_width), float(m_height));

	if (m_enableViewFrustumCulling)
		computeViewFrustum();

	// fill light and render queues
	fillQueues();

	// render all objects that support deferred shading
	renderDeferred();

	// the rest is rendered using good ol' forward rendering
	renderForward();
}

void RenderEngine::fillQueues()
{
	m_lightQueue.clear();

	// sort lights by type and priority
	for (const Light* light : m_lights) {
		if (!light->isActiveAndEnabled())
			continue;

		if (m_enableViewFrustumCulling && !checkIntersection(m_viewFrustum, light))
			continue;

		m_lightQueue.insert(light);
	}

	m_deferredQueue.clear();
	m_forwardQueue.clear();

	// sort objects into render paths and fill render queues
	for (const Renderer* renderer : m_renderers) {
		if (!(renderer->isActiveAndEnabled() && renderer->isVisible()))
			continue;

		const Transform* transform = renderer->getEntity()->getTransform();

		for (unsigned int i = 0; i < renderer->materialCount(); ++i) {

			const Material* material = renderer->getMaterial(i);
			if (!material)
				continue;

			const Effect* effect = material->getEffect();
			if (!effect)
				continue;

			const Drawable* obj = renderer->getDrawable(i);
			if (!obj)
				continue;

			// Frustum culling
			if (m_enableViewFrustumCulling && !checkIntersection(m_viewFrustum, transform, obj))
				continue;

			// if the object is not transparent and has a deferred pass, put it into deferred queue (deferred pass will be ignored in transparent effects)
			if (m_enableDeferred && (effect->getRenderType() != type_transparent)) {
				const Pass* passDeferred = effect->getPass(light_deferred);
				if (passDeferred && passDeferred->program) {
					m_deferredQueue.insert({ transform, obj, material, passDeferred, nullptr });
					continue;
				}
			}

			// if not, check if forward pass is present
			const Pass* passFwdBase = effect->getPass(light_forward_base);
			if (passFwdBase && passFwdBase->program) {
				const Light* firstLight = nullptr;

				auto lit = m_lightQueue.begin();
				if (!m_lightQueue.empty()) firstLight = *lit;
				++lit;

				m_forwardQueue.insert({ transform, obj, material, passFwdBase, firstLight });

				unsigned int lc = 1;

				// fill forward queue with limited amount of lights per object
				const Pass* passFwdAdd = effect->getPass(light_forward_add);
				if (passFwdAdd && passFwdAdd->program) {
					while ((lc <= m_maxFwdLights) && (lit != m_lightQueue.end())) {
						if (checkIntersection(*lit, transform, obj)) {
							m_forwardQueue.insert({ transform, obj, material, passFwdAdd, *lit });
							++lc;
						}
						++lit;
					}
				}

				continue;
			}

			// skip it otherwise (something probably went wrong while initializing this object)
		}
	}
}

void RenderEngine::renderDeferred()
{
	if (m_deferredQueue.empty() || !(m_deferredAmbientPass && m_deferredAmbientPass->program)) return;

	// G-Buffer pass
	deferredGPass();

	// Lighting pass
	deferredLPass();
}

void RenderEngine::deferredGPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufFBO);

	glClearBufferfv(GL_COLOR, 0, m_gClearColor);
	glClearBufferfv(GL_COLOR, 1, m_gClearColor);
	glClearBufferfv(GL_COLOR, 2, m_gClearColor);
	glClearBufferfv(GL_DEPTH, 0, &m_gClearDepth);

	const ShaderProgram* curProgram = nullptr;
	const Pass* curPass = nullptr;
	const Material* curMat = nullptr;
	const Drawable* curObj = nullptr;
	const Transform* curTransform = nullptr;

	for (const auto& job : m_deferredQueue) {
		if (job.program() != curProgram) {
			curProgram = job.program();
			curProgram->bind();
			// need to re-apply transform if program is new
			curTransform = nullptr;
		}

		if (job.pass != curPass) {
			curPass = job.pass;
			updateRenderState(curPass->state);
		}

		if (job.material != curMat) {
			curMat = job.material;
			curMat->apply(curProgram);
		}

		if (job.obj != curObj) {
			curObj = job.obj;
			curObj->bind();
		}

		if (job.transform != curTransform) {
			curTransform = job.transform;
			m_objUniforms.setPerObject(curTransform);
			m_objUniforms.apply(curProgram);
		}

		curObj->draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderEngine::deferredLPass()
{
	glBindVertexArray(m_lightMeshVAO);
	m_lightMeshIbuf.bind();

	auto ap = m_deferredAmbientPass;
	if (m_outputMode) ap = m_deferredDebugPass;

	if (!(ap && ap->program)) return;

	// Render ambient light (or draw g-buffer contents in debug mode)
	bindDeferredLightPass(ap);
	applyAmbient(true, ap->program);

	if (m_outputMode)
		ap->program->setUniform(g_mode_id, static_cast<int>(m_outputMode));

	drawDeferredLight(nullptr, ap->program);

	if (!(m_deferredLightPass && m_deferredLightPass->program) || m_outputMode) return;

	// Render normal lights
	bindDeferredLightPass(m_deferredLightPass);
	applyAmbient(false, m_deferredLightPass->program);

	for (const Light* light : m_lightQueue) {
		applyLight(light, m_deferredLightPass->program);
		drawDeferredLight(light, m_deferredLightPass->program);
	}
}

void RenderEngine::renderForward()
{
	if (m_forwardQueue.empty()) return;

	const ShaderProgram* curProgram = nullptr;
	const Pass* curPass = nullptr;
	const Material* curMat = nullptr;
	const Light* curLight = nullptr;
	const Drawable* curObj = nullptr;
	const Transform* curTransform = nullptr;

	for (const auto& job : m_forwardQueue) {
		if (job.program() != curProgram) {
			curProgram = job.program();
			curProgram->bind();
			// need to re-apply transform and light if program is new
			curTransform = nullptr;
			curLight = nullptr;
		}

		if (job.pass != curPass) {
			curPass = job.pass;
			updateRenderState(curPass->state);
			applyAmbient(curPass->mode == light_forward_base, curProgram);
		}

		if (job.material != curMat) {
			curMat = job.material;
			curMat->apply(curProgram);
		}

		if (job.obj != curObj) {
			curObj = job.obj;
			curObj->bind();
		}

		if (job.transform != curTransform) {
			curTransform = job.transform;
			m_objUniforms.setPerObject(curTransform);
			m_objUniforms.apply(curProgram);
		}

		if (job.light != curLight) {
			curLight = job.light;
			applyLight(curLight, curProgram);
		}

		curObj->draw();
	}
}

void RenderEngine::addEntity(const Entity* entity)
{
	const Renderer* r = entity->getComponent<Renderer>();
	if (r) {
		m_renderers.push_back(r);
	}

	const Light* l = entity->getComponent<Light>();
	if (l) {
		m_lights.push_back(l);
	}
}

void RenderEngine::removeEntity(const Entity* entity)
{
	const Renderer* r = entity->getComponent<Renderer>();
	if (r) {
		m_renderers.erase(std::remove(m_renderers.begin(), m_renderers.end(), r), m_renderers.end());
	}

	const Light* l = entity->getComponent<Light>();
	if (l) {
		m_lights.erase(std::remove(m_lights.begin(), m_lights.end(), l), m_lights.end());
	}
}

bool RenderEngine::checkIntersection(const Light* light, const Transform* transform, const Drawable* obj) const
{
	if (light->getType() == Light::type_directional) return true;

	const Transform* lTrans = light->getEntity()->getTransform();

	// get light position relative to renderer's coordinate system (without scale)
	glm::vec4 lp(lTrans->getPosition(), 1.0f);
	lp = transform->getInverseRigidMatrix() * lp;
	glm::vec3 lPos(lp);

	// apply renderer's scale directly to bounds
	aabb rBounds = obj->bounds();
	rBounds *= transform->getScale();

	// intersect aabb (renderer's bounds) with lights's sphere of influence
	return intersect_aabb_sphere(rBounds, { lPos, light->getRange() });
}

void RenderEngine::computeViewFrustum()
{
	for (unsigned int p = 0; p < 3; ++p) {
		for (unsigned int i = 0; i < 4; ++i) {
			m_viewFrustum.planes[p*2+0][i] = m_objUniforms.vp[i][3] + m_objUniforms.vp[i][p];
			m_viewFrustum.planes[p*2+1][i] = m_objUniforms.vp[i][3] - m_objUniforms.vp[i][p];
		}
	}

	m_viewFrustum.normalize();
}

bool RenderEngine::checkIntersection(const frustum& viewFrustum, const Transform* transform, const Drawable* obj) const
{
	glm::quat rot = transform->getRotation();
	aabb scaledBounds = obj->bounds() * glm::abs(transform->getScale());
	glm::vec3 rmin = rot * scaledBounds.min, rmax = rot * scaledBounds.max;
	obb rBounds{
		transform->getPosition() + (rmax + rmin) * 0.5f,
		scaledBounds.extents(),
		glm::mat3_cast(rot)
	};

	return intersect_obb_frustum(rBounds, viewFrustum);
}

bool RenderEngine::checkIntersection(const frustum& viewFrustum, const Light* light) const
{
	if (light->getType() == Light::type_directional) return true;

	const Transform* lTrans = light->getEntity()->getTransform();
	return intersect_sphere_frustum({ lTrans->getPosition(), light->getRange() }, viewFrustum);
}

void RenderEngine::bindDeferredLightPass(const Pass* pass)
{
	updateRenderState(pass->state);

	const ShaderProgram* program = pass->program;
	program->bind();
	program->setUniform(g_cm_mat_ivp_id, m_objUniforms.ivp);
	program->setUniform(g_cm_cam_pos_id, m_objUniforms.camPos);
	program->setTexture(g_gbuf_diffuse_id, m_gBufDiff.get());
	program->setTexture(g_gbuf_specSmooth_id, m_gBufSpec.get());
	program->setTexture(g_gbuf_normal_id, m_gBufNorm.get());
	program->setTexture(g_gbuf_depth_id, m_gBufDepth.get());
}

void RenderEngine::drawDeferredLight(const Light* light, const ShaderProgram* program)
{
	glm::mat4 transform;
	auto o = m_quadOffset;
	auto c = m_quadCount;

	if (light) {
		Light::type t = light->getType();

		if (t != Light::type_directional) {
			float r = light->getRange();
			float lr = r * m_lightMeshRadius;
			const Transform* lt = light->getEntity()->getTransform();
			glm::vec4 lp = {lt->getPosition(), 1.0f};
			glm::vec3 lpos = m_objUniforms.view * lp;
			
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

void RenderEngine::applyLight(const Light* light, const ShaderProgram* program)
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

void RenderEngine::applyAmbient(bool enabled, const ShaderProgram* program)
{
	program->setUniform(g_cm_light_ambient_id, enabled ? m_ambientLight : glm::vec4(0.f));
}

void RenderEngine::updateRenderState(const RenderState& newState)
{
	newState.differentialApply(m_renderState);
	m_renderState = newState;
}

void RenderEngine::uniforms_per_obj::setPerFrame(const Camera* camera, float w, float h)
{
	const Transform* camTrans = camera->getEntity()->getTransform();
	proj = camera->getProjectionMatrix(w, h);
	view = camTrans->getInverseRigidMatrix();
	vp = proj * view;
	ivp = glm::inverse(vp);
	camPos = camTrans->getPosition();
}

void RenderEngine::uniforms_per_obj::setPerObject(const Transform* transform)
{
	world = transform->getMatrix();
	tiworld = glm::transpose(glm::inverse(world));
	wvp = vp * world;
}

void RenderEngine::uniforms_per_obj::apply(const ShaderProgram* program) const
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

inline const Effect * RenderEngine::render_job::effect() const { return material->getEffect(); }
inline int RenderEngine::render_job::priority() const { return effect()->getQueuePriority(); }
inline unsigned int RenderEngine::render_job::lightMode() const { return pass->mode; }
inline const ShaderProgram * RenderEngine::render_job::program() const { return pass->program; }


void RenderEngine::debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	auto msg = boost::format("OpenGL Message [%s]: %s %s (ID %i): %s")
		% dbgSeverityString(severity)
		% dbgSourceString(source)
		% dbgTypeString(type)
		% id
		% message;

	std::cout << msg << std::endl;
}

void GLAPIENTRY glDbgMsg(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	RenderEngine::debugMessage(source, type, id, severity, length, message, userParam);
}

std::string RenderEngine::dbgSourceString(GLenum v)
{
	switch (v) {
	case GL_DEBUG_SOURCE_API:
		return "API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "Window System";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "Shader Compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "Third Party";
	case GL_DEBUG_SOURCE_APPLICATION:
		return "Application";
	case GL_DEBUG_SOURCE_OTHER:
		return "Other";
	default:
		return "Unknown";
	}
}

std::string RenderEngine::dbgTypeString(GLenum v)
{
	switch (v) {
	case GL_DEBUG_TYPE_ERROR:
		return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "Deprecated Behaviour";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "Undefined Behaviour";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "Portability";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "Performance";
	case GL_DEBUG_TYPE_MARKER:
		return "Marker";
	case GL_DEBUG_TYPE_OTHER:
		return "Other";
	default:
		return "Unknown";
	}
}

std::string RenderEngine::dbgSeverityString(GLenum v)
{
	switch (v) {
	case GL_DEBUG_SEVERITY_HIGH:
		return "High";
	case GL_DEBUG_SEVERITY_MEDIUM:
		return "Medium";
	case GL_DEBUG_SEVERITY_LOW:
		return "Low";
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return "Notification";
	default:
		return "Unknown";
	}
}
