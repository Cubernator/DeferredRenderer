#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP

#include "core/ComponentModule.hpp"

#include "util/singleton.hpp"

#include "graphics/Buffer.hpp"
#include "graphics/Light.hpp"
#include "graphics/RenderState.hpp"
#include "util/bounds.hpp"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/indexed_by.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/multi_index/composite_key.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/mem_fun.hpp"

#include <unordered_set>
#include <set>
#include <vector>

namespace mi = boost::multi_index;

class Engine;
class Entity;
class Transform;
class Renderer;
class Drawable;
class Material;
class Effect;
class Pass;
class Light;
class Camera;
class ShaderProgram;
class Texture2D;
class FrameBuffer;
class RenderTexture;
class ImageEffect;

class RenderEngine : public singleton<RenderEngine>, public ComponentModule
{
public:
	enum output_mode
	{
		output_default,
		output_diffuse,
		output_specular,
		output_smoothness,
		output_normal,
		output_depth,
		output_mode_max
	};

	explicit RenderEngine(Engine *parent);
	~RenderEngine();

	Engine *parent() { return m_parent; }
	const Engine *parent() const { return m_parent; }

	int screenWidth() const { return m_width; }
	int screenHeight() const { return m_height; }

	bool isDeferredEnabled() const { return m_enableDeferred; }
	void setDeferredEnabled(bool val) { m_enableDeferred = val; }

	bool isVFCEnabled() const { return m_enableViewFrustumCulling; }
	void setVFCEnabled(bool val) { m_enableViewFrustumCulling = val; }

	output_mode outputMode() const { return m_outputMode; }
	void setOutputMode(output_mode val) { m_outputMode = val; }

	float avgLightsPerObj() const { return m_avgLightsPerObj; }
	std::size_t triangleCount() const { return m_triangleCount; }

	void setConvertToSRGB(bool l);

	void blit(const Texture2D* source, const RenderTexture* dest, const Material* material = nullptr, std::size_t passIndex = 0);

	const RenderTexture* getAuxRenderTexture();

	virtual void addComponent(Component* cmpt) final;
	virtual void removeComponent(Component* cmpt) final;

	void render();

	void onResize(int width, int height);

private:
	struct uniforms_per_obj
	{
		glm::mat4 world, view, proj;
		glm::mat4 vp, wvp;
		glm::mat4 ivp;
		glm::mat4 tiworld;
		glm::vec3 camPos;

		void setPerFrame(const Camera* camera, float w, float h);
		void setPerObject(const Transform* transform);
		void apply(const ShaderProgram* program) const;
	};

	struct render_job
	{
		const Transform* transform;
		const Drawable* obj;
		const Material* material;
		const Pass* pass;
		const Light* light;

		const Effect* effect() const;
		int priority() const;
		unsigned int lightMode() const;
		const ShaderProgram* program() const;
	};

	struct deferred_queue_indices : public mi::indexed_by<
		mi::ordered_non_unique<mi::composite_key<render_job,
			mi::const_mem_fun<render_job, int, &render_job::priority>,
			mi::const_mem_fun<render_job, const ShaderProgram*, &render_job::program>,
			mi::member<render_job, const Pass*, &render_job::pass>,
			mi::member<render_job, const Material*, &render_job::material>,
			mi::member<render_job, const Drawable*, &render_job::obj>
		>>
	> { };

	using deferred_queue = mi::multi_index_container<
		render_job,
		deferred_queue_indices
	>;

	struct forward_queue_indices : public mi::indexed_by<
		mi::ordered_non_unique<mi::composite_key<render_job,
			mi::const_mem_fun<render_job, int, &render_job::priority>,
			mi::const_mem_fun<render_job, const ShaderProgram*, &render_job::program>,
			mi::const_mem_fun<render_job, unsigned int, &render_job::lightMode>,
			mi::member<render_job, const Pass*, &render_job::pass>,
			mi::member<render_job, const Material*, &render_job::material>,
			mi::member<render_job, const Drawable*, &render_job::obj>
		>>
	> { };

	using forward_queue = mi::multi_index_container<
		render_job,
		forward_queue_indices
	>;

	struct light_queue_indices : public mi::indexed_by<
		mi::ordered_non_unique<mi::composite_key<Light,
			mi::const_mem_fun<Light, Light::light_type, &Light::type>,
			mi::const_mem_fun<Light, int, &Light::priority>
		>>
	> { };

	using light_queue = mi::multi_index_container<
		const Light*,
		light_queue_indices
	>;

	Engine *m_parent;

	std::vector<const Renderer*> m_renderers;
	std::vector<const Light*> m_lights;

	std::vector<ImageEffect*> m_imgEffects, m_activeImgEffects;

	int m_width, m_height;

	Camera* m_camera;
	frustum m_viewFrustum;

	glm::vec4 m_clearColor;

	deferred_queue m_deferredQueue;
	forward_queue m_forwardQueue;

	std::unique_ptr<FrameBuffer> m_gFrameBuffer;
	std::unique_ptr<Texture2D> m_gBufDiff, m_gBufSpec, m_gBufNorm, m_gBufDepth;

	std::unique_ptr<RenderTexture> m_accBuffer;
	std::vector<std::unique_ptr<RenderTexture>> m_auxBuffers;

	std::unique_ptr<Effect> m_deferredLightEffect;
	const Pass* m_deferredAmbientPass;
	const Pass* m_deferredLightPass;
	const Pass* m_deferredDebugPass;

	GLuint m_lightMeshVAO;
	VertexBuffer<glm::vec3> m_lightMeshVbuf;
	IndexBuffer<unsigned int> m_lightMeshIbuf;
	float m_lightMeshRadius;
	std::size_t m_quadOffset, m_sphereOffset;
	std::size_t m_quadCount, m_sphereCount;

	GLuint m_fsQuadVAO;
	VertexBuffer<float> m_fsQuadVbuf;
	std::unique_ptr<Material> m_copyMat;
	RenderTexture* m_currentSourceBuf;

	light_queue m_lightQueue;

	RenderState m_renderState;
	uniforms_per_obj m_objUniforms;
	unsigned int m_maxFwdLights;
	glm::vec4 m_ambientLight;

	bool m_enableDeferred;
	bool m_enableViewFrustumCulling;
	output_mode m_outputMode;

	float m_avgLightsPerObj;
	std::size_t m_triangleCount;


	void setupDeferredPath();
	void createCombinedLightMesh();
	void createPPResources();
	void createDefaultResources();

	void getImgEffects();
	void fillQueues();
	void geometryPass();
	void lightingPass();
	void forwardPass();
	void postProcessing();

	void applyLight(const Light* light, const ShaderProgram* program);
	void applyAmbient(bool enabled, const ShaderProgram* program);
	void updateRenderState(const RenderState& newState);
	void bindDeferredLightPass(const Pass* pass);
	void drawDeferredLight(const Light* light, const ShaderProgram* program);

	void computeViewFrustum();

	bool checkIntersection(const Light* light, const Transform* transform, const Drawable* obj) const;
	bool checkIntersection(const frustum& viewFrustum, const Transform* transform, const Drawable* obj) const;
	bool checkIntersection(const frustum& viewFrustum, const Light* light) const;


	// cppcheck-suppress unusedPrivateFunction
	static void debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	friend void GLAPIENTRY glDbgMsg(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	static std::string dbgSourceString(GLenum v);
	static std::string dbgTypeString(GLenum v);
	static std::string dbgSeverityString(GLenum v);
};

#endif // RENDERENGINE_HPP