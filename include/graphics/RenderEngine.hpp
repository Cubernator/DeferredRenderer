#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP

#include "graphics/Effect.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/render_state.hpp"

#include <unordered_set>
#include <set>
#include <vector>

class Engine;
class Entity;
class Renderer;
class Light;
class Camera;
class ShaderProgram;
class Texture2D;

class RenderEngine
{
public:
	RenderEngine(Engine *parent);
	~RenderEngine();

	Engine *getParent() { return m_parent; }
	const Engine *getParent() const { return m_parent; }

	bool isDeferredEnabled() const { return m_enableDeferred; }
	void setDeferredEnabled(bool val) { m_enableDeferred = val; }

	void render();

	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);

	void onResize(int width, int height);

	static RenderEngine* instance() { return s_instance; }

private:
	struct light_priority_comparer
	{
		bool operator() (const Light* lhs, const Light* rhs) const;
	};

	struct uniforms_per_obj
	{
		glm::mat4 world, view, proj;
		glm::mat4 vp, wvp;
		glm::mat4 ivp;
		glm::mat4 tiworld;
		glm::vec3 camPos;
		glm::vec4 ambientLight;

		void setProjView(Camera* camera, float w, float h);
		void setWorld(Entity* entity);
		void apply(ShaderProgram* program) const;
	};

	struct render_obj
	{
		Renderer* renderer;
		const Effect::pass* pass;

		render_obj(Renderer* r, const Effect::pass* p) : renderer(r), pass(p) { }
	};

	using light_queue = std::multiset<Light*, light_priority_comparer>;
	using renderer_queue = std::vector<render_obj>;

	Engine *m_parent;
	std::vector<Renderer*> m_renderers;
	std::vector<Light*> m_lights;

	glm::vec4 m_clearColor;

	renderer_queue m_deferredQueue, m_forwardQueue;

	GLuint m_gBufFBO;
	std::unique_ptr<Texture2D> m_gBufDiff, m_gBufSpec, m_gBufNorm, m_gBufDepth;
	GLenum m_gDrawBufs[3];
	float m_gClearColor[4];
	float m_gClearDepth;

	std::unique_ptr<Effect> m_deferredLightEffect;
	const Effect::pass* m_deferredAmbientPass;
	const Effect::pass* m_deferredLightPass;
	VertexBuffer<glm::vec3> m_quadVBuf;
	GLuint m_quadVAO;

	light_queue m_dirLights, m_posLights;
	std::vector<Light*> m_lightQueue;

	render_state m_renderState;
	uniforms_per_obj m_objUniforms;
	unsigned int m_maxLights;

	bool m_enableDeferred;

	static RenderEngine* s_instance;


	void setupDeferredPath();

	void renderDeferred();
	void renderForward();

	void bindPass(const Effect::pass* pass, Material* material);
	void applyLight(Light* light, ShaderProgram* program);
	void updateRenderState(const render_state& newState);
	void bindDeferredLightPass(const Effect::pass* pass);
};

#endif // RENDERENGINE_HPP