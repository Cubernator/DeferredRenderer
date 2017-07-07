#include "RenderEngine.hpp"

#include "core/Engine.hpp"
#include "core/Scene.hpp"
#include "core/Entity.hpp"
#include "core/Transform.hpp"
#include "core/ObjectRegistry.hpp"
#include "core/app_info.hpp"
#include "Material.hpp"
#include "Effect.hpp"
#include "shader/Shader.hpp"
#include "shader/ShaderProgram.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "texture/Texture2D.hpp"
#include "texture/RenderTexture.hpp"
#include "FrameBuffer.hpp"
#include "ImageEffect.hpp"
#include "util/intersection_tests.hpp"
#include "scripting/class_registry.hpp"

#include "boost/format.hpp"
#include "GL/glew.h"

#define NUM_AUX_BUFFERS 2

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

// Deferred Debugger parameters
DEF_UNIFORM_ID(mode);
DEF_UNIFORM_ID(nearPlane);
DEF_UNIFORM_ID(farPlane);

// Image Effect parameters
DEF_UNIFORM_ID(img_source);
DEF_UNIFORM_ID(img_resolution);


RenderEngine::RenderEngine(Engine* parent)
	: m_parent(parent), m_camera(nullptr), m_lightMeshVAO(0), m_fsQuadVAO(0), m_currentSourceBuf(nullptr),
	m_deferredAmbientPass(nullptr), m_deferredLightPass(nullptr),
	m_enableDeferred(true), m_enableViewFrustumCulling(true),
	m_outputMode(output_default), m_avgLightsPerObj(0.0f), m_triangleCount(0)
{
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&glDbgMsg, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

	int mfl = app_info::get<int>("maxForwardLights", 8);
	m_maxFwdLights = (mfl < 0) ? std::numeric_limits<unsigned int>::max() : mfl;

	setupDeferredPath();
	createDefaultResources();
	createPPResources();
	m_renderState.apply();
}

void RenderEngine::setupDeferredPath()
{
	m_gBufDiff = std::make_unique<Texture2D>();
	m_gBufDiff->setParams(false, filter_point, wrap_clampToEdge);

	m_gBufSpec = std::make_unique<Texture2D>();
	m_gBufSpec->setParams(false, filter_point, wrap_clampToEdge);

	m_gBufNorm = std::make_unique<Texture2D>();
	m_gBufNorm->setParams(false, filter_point, wrap_clampToEdge);

	m_gBufDepth = std::make_unique<Texture2D>();
	m_gBufDepth->setParams(false, filter_point, wrap_clampToEdge);

	m_accBuffer = std::make_unique<RenderTexture>();
	m_accBuffer->setParams(false, filter_bilinear, wrap_clampToEdge);

	for (unsigned int i = 0; i < NUM_AUX_BUFFERS; ++i) {
		m_auxBuffers.push_back(std::make_unique<RenderTexture>());
		m_auxBuffers[i]->setParams(false, filter_bilinear, wrap_clampToEdge);
	}

	onResize(m_parent->screenWidth(), m_parent->screenHeight());

	FrameBuffer::target_array gTargets;
	gTargets.push_back(make_tex2D_tgt(m_gBufDiff.get()));
	gTargets.push_back(make_tex2D_tgt(m_gBufSpec.get()));
	gTargets.push_back(make_tex2D_tgt(m_gBufNorm.get()));

	m_gFrameBuffer = std::make_unique<FrameBuffer>();
	m_gFrameBuffer->setTargetsDepth(make_tex2D_tgt(m_gBufDepth.get()), std::move(gTargets));

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
	b = unsigned int(vertices.size());
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
	b = unsigned int(vertices.size());

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
	b = unsigned int(vertices.size());
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

void RenderEngine::createPPResources()
{
	std::string vs = R"vs(
#version 330
layout(location = 0) in vec4 inPos;
layout(location = 1) in vec2 inUV;
out vec2 uv;
void main()
{
	gl_Position = inPos;
	uv = inUV;
}
)vs";

	std::string fs = R"fs(
#version 330
uniform sampler2D img_source;
in vec2 uv;
layout(location = 0) out vec4 f_output;
void main()
{
	f_output = texture(img_source, uv);
}
)fs";

	nlohmann::json copyEffectJson = R"(
{
	"name" : "builtin_copy",
	"passes" : [
		{
			"name" : "default",
			"state": {
				"cull" : false,
				"depthTest" : false,
				"depthWrite" : false
			},
			"program" : {
				"name" : "builtin_copy", 
				"shaders" : [ "builtin_copy.vert", "builtin_copy.frag" ]
			}
		}
	]
}
)"_json;

	auto objReg = ObjectRegistry::instance();

	auto copyVS = objReg->emplace<Shader>("builtin_copy.vert", Shader::type_vertex, vs);
	auto copyFS = objReg->emplace<Shader>("builtin_copy.frag", Shader::type_fragment, fs);

	auto copyEffect = content::get_pooled_json<Effect>(copyEffectJson);

	m_copyMat = std::make_unique<Material>();
	m_copyMat->setEffect(copyEffect);

	std::vector<float> fsQuadVertices{
		-1.f, -1.f, -1.f, 1.f, 0.f, 0.f,
		1.f, -1.f, -1.f, 1.f, 1.f, 0.f,
		-1.f, 1.f, -1.f, 1.f, 0.f, 1.f,
		1.f, 1.f, -1.f, 1.f, 1.f, 1.f
	};
	m_fsQuadVbuf.setData(fsQuadVertices.size(), fsQuadVertices.data());

	glGenVertexArrays(1, &m_fsQuadVAO);
	glBindVertexArray(m_fsQuadVAO);
	m_fsQuadVbuf.bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 24, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 24, (const void*)16);
	glBindVertexArray(0);
}

void RenderEngine::createDefaultResources()
{
	auto objReg = ObjectRegistry::instance();

	pixel::srgb pw{ 255U, 255U, 255U };
	auto texWhite = std::make_unique<Texture2D>();
	texWhite->setParams(false, filter_point, wrap_repeat);
	texWhite->setData(&pw, 1, 1);
	texWhite->setName("white");
	objReg->add(std::move(texWhite));

	pixel::srgb pb{ 0U, 0U, 0U };
	auto texBlack = std::make_unique<Texture2D>();
	texBlack->setParams(false, filter_point, wrap_repeat);
	texBlack->setData(&pb, 1, 1);
	texBlack->setName("black");
	objReg->add(std::move(texBlack));
}

RenderEngine::~RenderEngine()
{
	if (m_lightMeshVAO) {
		glDeleteVertexArrays(1, &m_lightMeshVAO);
	}
	if (m_fsQuadVAO) {
		glDeleteVertexArrays(1, &m_fsQuadVAO);
	}
}

void RenderEngine::onResize(int width, int height)
{
	m_width = width;
	m_height = height;

	glViewport(0, 0, m_width, m_height);

	m_gBufDiff->setData<pixel::rgb8>(nullptr, m_width, m_height);
	m_gBufSpec->setData<pixel::rgba8>(nullptr, m_width, m_height);
	m_gBufNorm->setData<pixel::rgb10a2>(nullptr, m_width, m_height);
	m_gBufDepth->setData<pixel::depth24>(nullptr, m_width, m_height);

	m_accBuffer->setData<pixel::rgba16f>(m_width, m_height, RenderTexture::depth_24);

	for (auto& rt : m_auxBuffers) {
		rt->setData<pixel::rgba8>(m_width, m_height);
	}
}

void RenderEngine::render()
{
	Scene* scene = m_parent->scene();
	if (scene) {
		m_clearColor = scene->backColor();
		m_ambientLight = scene->ambientLight();
	}

	getImgEffects();

	if (!m_camera || !m_camera->isActiveAndEnabled()) return; // can't render anything without an active camera!

	m_objUniforms.setPerFrame(m_camera, float(m_width), float(m_height));

	if (m_enableViewFrustumCulling)
		computeViewFrustum();

	// fill light and render queues
	fillQueues();

	geometryPass();

	m_accBuffer->fbo()->bind();
	m_accBuffer->fbo()->setClearColor(0, m_clearColor);
	m_accBuffer->fbo()->clear();

	lightingPass();

	forwardPass();

	postProcessing();
}

void RenderEngine::getImgEffects()
{
	Camera* oldCam = m_camera;
	m_camera = Camera::main();

	if (m_camera != oldCam) {
		if (m_camera) {
			m_imgEffects = m_camera->entity()->getComponents<ImageEffect>();
		} else {
			m_imgEffects.clear();
		}
	}

	m_activeImgEffects.clear();
	if (m_camera && m_camera->isActiveAndEnabled()) {
		std::copy_if(
			m_imgEffects.begin(), m_imgEffects.end(),
			std::back_inserter(m_activeImgEffects),
			[](ImageEffect* e) { return e->isEnabled(); }
		);
	}
}

void RenderEngine::fillQueues()
{
	unsigned int sampleAcc = 0, sampleCount = 0;
	m_triangleCount = 0;

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

		const Transform* transform = renderer->entity()->transform();

		for (unsigned int i = 0; i < renderer->materialCount(); ++i) {

			const Material* material = renderer->material(i);
			if (!material)
				continue;

			const Effect* effect = material->effect();
			if (!effect)
				continue;

			const Drawable* obj = renderer->getDrawable(i);
			if (!obj)
				continue;

			// Frustum culling
			if (m_enableViewFrustumCulling && !checkIntersection(m_viewFrustum, transform, obj))
				continue;

			m_triangleCount += obj->triangles();

			// if the object is not transparent and has a deferred pass, put it into deferred queue (deferred pass will be ignored in transparent effects)
			if (m_enableDeferred && (effect->renderType() != type_transparent)) {
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

				sampleAcc += lc;
				++sampleCount;

				continue;
			}

			// skip it otherwise (something probably went wrong while initializing this object)
		}
	}

	m_avgLightsPerObj = float(sampleAcc) / float(sampleCount);
}

void RenderEngine::geometryPass()
{
	if (m_deferredQueue.empty()) return;

	m_gFrameBuffer->bind();
	m_gFrameBuffer->clear();

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
}

void RenderEngine::lightingPass()
{
	if (m_deferredQueue.empty()) return;
	if (!(m_deferredAmbientPass && m_deferredAmbientPass->program)) return;

	glBindVertexArray(m_lightMeshVAO);
	m_lightMeshIbuf.bind();

	auto ap = m_deferredAmbientPass;
	if (m_outputMode) ap = m_deferredDebugPass;

	if (!(ap && ap->program)) return;

	// Render ambient light (or draw g-buffer contents in debug mode)
	bindDeferredLightPass(ap);
	applyAmbient(true, ap->program);

	if (m_outputMode) {
		ap->program->setUniform(g_mode_id, static_cast<int>(m_outputMode));
		ap->program->setUniform(g_nearPlane_id, m_camera->nearPlane());
		ap->program->setUniform(g_farPlane_id, m_camera->farPlane());
	}

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

void RenderEngine::forwardPass()
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

void RenderEngine::postProcessing()
{
	if (m_activeImgEffects.size() == 0) {
		// if no image effects are active, just copy the accumulation buffer to the backbuffer
		setConvertToSRGB(true);
		blit(m_accBuffer.get(), nullptr);
	} else {
		RenderTexture* input = m_accBuffer.get();
		RenderTexture* output = m_auxBuffers[0].get();

		for (unsigned int i = 0; i < m_activeImgEffects.size(); ++i) {
			m_currentSourceBuf = input;
			if (i == m_activeImgEffects.size() - 1)
				output = nullptr; // the last image effect should write directly into the backbuffer

			setConvertToSRGB(true);
			m_activeImgEffects[i]->apply(input, output);

			std::swap(input, output);
		}
	}
}

void RenderEngine::blit(const Texture2D* source, const RenderTexture* dest, const Material* material, std::size_t passIndex)
{
	if (!material)
		material = m_copyMat.get();

	auto effect = material->effect();
	if (effect && (passIndex < effect->passCount())) {
		auto pass = effect->getPass(passIndex);
		if (pass && pass->program) {
			if (dest) {
				dest->fbo()->bind();
			} else {
				FrameBuffer::unbind();
			}

			pass->program->bind();
			updateRenderState(pass->state);

			material->apply(pass->program);
			pass->program->setTexture(g_img_source_id, source);

			if (source) {
				pass->program->setUniform(g_img_resolution_id, glm::vec4{
					source->width(), source->height(), 1.f / source->width(), 1.f / source->height()
				});
			}

			glBindVertexArray(m_fsQuadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}
}

const RenderTexture* RenderEngine::getAuxRenderTexture()
{
	for (auto& rt : m_auxBuffers) {
		if (rt.get() != m_currentSourceBuf) { // avoid returning currently used source buffer
			return rt.get();
		}
	}
	return nullptr;
}

void RenderEngine::setConvertToSRGB(bool l)
{
	if (l) {
		glEnable(GL_FRAMEBUFFER_SRGB);
	} else {
		glDisable(GL_FRAMEBUFFER_SRGB);
	}
}

void RenderEngine::addComponent(Component* cmpt)
{
	auto r = dynamic_cast<Renderer*>(cmpt);
	if (r) {
		m_renderers.push_back(r);
	}

	auto l = dynamic_cast<Light*>(cmpt);
	if (l) {
		m_lights.push_back(l);
	}
}

void RenderEngine::removeComponent(Component* cmpt)
{
	auto r = dynamic_cast<Renderer*>(cmpt);
	if (r) {
		m_renderers.erase(std::remove(m_renderers.begin(), m_renderers.end(), r), m_renderers.end());
	}

	auto l = dynamic_cast<Light*>(cmpt);
	if (l) {
		m_lights.erase(std::remove(m_lights.begin(), m_lights.end(), l), m_lights.end());
	}
}

bool RenderEngine::checkIntersection(const Light* light, const Transform* transform, const Drawable* obj) const
{
	if (light->type() == Light::type_directional) return true;

	const Transform* lTrans = light->entity()->transform();

	// get light position relative to renderer's coordinate system (without scale)
	glm::vec4 lp(lTrans->position(), 1.0f);
	lp = transform->getInverseRigidMatrix() * lp;
	glm::vec3 lPos(lp);

	// apply renderer's scale directly to bounds
	aabb rBounds = obj->bounds();
	rBounds *= transform->scale();

	// intersect aabb (renderer's bounds) with lights's sphere of influence
	return intersect_aabb_sphere(rBounds, { lPos, light->range() });
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
	glm::quat rot = transform->rotation();
	aabb scaledBounds = obj->bounds() * glm::abs(transform->scale());
	glm::vec3 rmin = rot * scaledBounds.min, rmax = rot * scaledBounds.max;
	obb rBounds{
		transform->position() + (rmax + rmin) * 0.5f,
		scaledBounds.extents(),
		glm::mat3_cast(rot)
	};

	return intersect_obb_frustum(rBounds, viewFrustum);
}

bool RenderEngine::checkIntersection(const frustum& viewFrustum, const Light* light) const
{
	if (light->type() == Light::type_directional) return true;

	const Transform* lTrans = light->entity()->transform();
	return intersect_sphere_frustum({ lTrans->position(), light->range() }, viewFrustum);
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
		Light::light_type t = light->type();

		if (t != Light::type_directional) {
			float r = light->range();
			float lr = r * m_lightMeshRadius;
			const Transform* lt = light->entity()->transform();
			glm::vec4 lp = {lt->position(), 1.0f};
			glm::vec3 lpos = m_objUniforms.view * lp;
			
			if ((-lpos.z - lr) > m_camera->nearPlane()) {
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
	glDrawElements(GL_TRIANGLES, GLsizei(c), GL_UNSIGNED_INT, reinterpret_cast<void*>(o));
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
	const Transform* camTrans = camera->entity()->transform();
	proj = camera->getProjectionMatrix(w, h);
	view = camTrans->getInverseRigidMatrix();
	vp = proj * view;
	ivp = glm::inverse(vp);
	camPos = camTrans->position();
}

void RenderEngine::uniforms_per_obj::setPerObject(const Transform* transform)
{
	world = transform->getMatrix();
	// TODO: use SIMD commands to speed this up
	tiworld = glm::transpose(transform->getInverseMatrix());
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

inline const Effect * RenderEngine::render_job::effect() const { return material->effect(); }
inline int RenderEngine::render_job::priority() const { return effect()->queuePriority(); }
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

SCRIPTING_REGISTER_STATIC_CLASS(Graphics)

SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, screenWidth, RenderEngine)
SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, screenHeight, RenderEngine)

SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, isDeferredEnabled, RenderEngine)
SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, setDeferredEnabled, RenderEngine)

SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, isVFCEnabled, RenderEngine)
SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, setVFCEnabled, RenderEngine)

SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, outputMode, RenderEngine)
SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, setOutputMode, RenderEngine)

SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, avgLightsPerObj, RenderEngine)
SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, triangleCount, RenderEngine)

SCRIPTING_AUTO_MODULE_METHOD_C(Graphics, setConvertToSRGB, RenderEngine)
