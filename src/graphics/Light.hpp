#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "core/Component.hpp"
#include "util/json_interpreter.hpp"

#include "glm.hpp"

class Renderer;

class Light : public Component
{
public:
	enum light_type
	{
		type_directional = 0,
		type_point = 1,
		type_spot = 2
	};

	explicit Light(Entity* parent);

	light_type type() const { return m_type; }
	const glm::vec4& color() const { return m_color; }
	float intensity() const { return m_intensity; }
	float range() const { return m_range; }
	float spotAngle() const { return m_spotAngle; }
	float spotFalloff() const { return m_spotFalloff; }
	int priority() const { return m_priority; }

	void setType(light_type val) { m_type = val; }
	void setColor(const glm::vec4& val) { m_color = val; }
	void setIntensity(float val) { m_intensity = val; }
	void setRange(float val) { m_range = val; }
	void setSpotAngle(float val) { m_spotAngle = val; }
	void setSpotFalloff(float val) { m_spotFalloff = val; }
	void setPriority(int val) { m_priority = val; }

	bool isVisible() const;

	void getUniforms(glm::vec4& color, glm::vec4& dir, glm::vec4& atten, glm::vec4& spot) const;

protected:
	virtual void apply_json_impl(const nlohmann::json& json) override;

private:
	light_type m_type;
	glm::vec4 m_color;
	float m_intensity;
	float m_range;
	float m_spotAngle;
	float m_spotFalloff;
	int m_priority;
};

#endif // LIGHT_HPP