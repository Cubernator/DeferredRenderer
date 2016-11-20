#ifndef FORWARDRENDERENGINE_HPP
#define FORWARDRENDERENGINE_HPP

#include "RenderEngine.hpp"
#include "graphics/Effect.hpp"
#include "graphics/render_state.hpp"

#include <unordered_set>
#include <set>
#include <vector>

class Entity;
class Renderer;
class Light;
class Camera;
class ShaderProgram;

class ForwardRenderEngine : public RenderEngine
{
public:
	ForwardRenderEngine(Engine* parent);

protected:
	virtual void onRender() override;
	virtual void onAddEntity(Entity* entity) override;
	virtual void onRemoveEntity(Entity* entity) override;

private:
	struct light_priority_comparer
	{
		bool operator() (const Light* lhs, const Light* rhs) const;
	};

	struct uniforms_per_obj
	{
		glm::mat4 world, view, proj;
		glm::mat4 vp, wvp;

		void setProjView(Camera* camera, float w, float h);
		void setWorld(Entity* entity);
		void apply(ShaderProgram* program) const;
	};

	std::unordered_set<Light*> m_lights;
	std::multiset<Light*, light_priority_comparer> m_dirLights;
	std::multiset<Light*, light_priority_comparer> m_posLights;
	std::vector<Light*> m_lightQueue;

	render_state m_renderState;
	uniforms_per_obj m_objUniforms;

	static uniform_id s_cm_mat_proj_id,
		s_cm_mat_view_id,
		s_cm_mat_world_id,
		s_cm_mat_vp_id,
		s_cm_mat_wvp_id,
		s_cm_light_color_id,
		s_cm_light_dir_id,
		s_cm_light_radius_id;

	unsigned int m_maxLights;

	void bindPass(const Effect::pass* pass, Material* material);
	void applyLight(Light* light, ShaderProgram* program);
	void updateRenderState(const render_state& newState);
};

#endif // FORWARDRENDERENGINE_HPP