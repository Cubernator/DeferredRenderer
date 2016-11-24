#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP

#include "graphics/Effect.hpp"
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

class RenderEngine
{
public:
	RenderEngine(Engine *parent);

	Engine *getParent() { return m_parent; }
	const Engine *getParent() const { return m_parent; }

	void render();

	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);

private:
	struct light_priority_comparer
	{
		bool operator() (const Light* lhs, const Light* rhs) const;
	};

	struct uniforms_per_obj
	{
		glm::mat4 world, view, proj;
		glm::mat4 vp, wvp;
		glm::mat4 tiworld;
		glm::vec3 camPos;

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

	renderer_queue m_deferredQueue, m_forwardQueue;

	light_queue m_dirLights, m_posLights;
	std::vector<Light*> m_lightQueue;

	render_state m_renderState;
	uniforms_per_obj m_objUniforms;
	unsigned int m_maxLights;

	static uniform_id s_cm_mat_proj_id,
		s_cm_mat_view_id,
		s_cm_mat_world_id,
		s_cm_mat_vp_id,
		s_cm_mat_wvp_id,
		s_cm_mat_tiworld_id,
		s_cm_cam_pos_id,
		s_cm_light_color_id,
		s_cm_light_dir_id,
		s_cm_light_radius_id;

	void renderDeferred();
	void renderForward();

	void bindPass(const Effect::pass* pass, Material* material);
	void applyLight(Light* light, ShaderProgram* program);
	void updateRenderState(const render_state& newState);
};

#endif // RENDERENGINE_HPP