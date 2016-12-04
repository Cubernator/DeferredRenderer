#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "core/Component.hpp"
#include "util/json_interpreter.hpp"

#include "glm.hpp"

class Renderer;

class Light : public Component
{
public:
	enum type
	{
		type_directional = 0,
		type_point = 1,
		type_spot = 2
	};

	Light(Entity* parent);

	type getType() const { return m_type; }
	const glm::vec4& getColor() const { return m_color; }
	float getIntensity() const { return m_intensity; }
	float getRange() const { return m_range; }
	float getSpotAngle() const { return m_spotAngle; }
	float getSpotFalloff() const { return m_spotFalloff; }
	int getPriority() const { return m_priority; }

	void setType(type val) { m_type = val; }
	void setColor(const glm::vec4& val) { m_color = val; }
	void setIntensity(float val) { m_intensity = val; }
	void setRange(float val) { m_range = val; }
	void setSpotAngle(float val) { m_spotAngle = val; }
	void setSpotFalloff(float val) { m_spotFalloff = val; }
	void setPriority(int val) { m_priority = val; }

	bool isVisible() const;
	/*
	glm::vec4 getDirUniform() const;
	glm::vec4 getSpotUniform() const;
	glm::vec4 getAttenUniform() const;
	glm::vec4 getPremultipliedColor() const;
	*/

	void getUniforms(glm::vec4& color, glm::vec4& dir, glm::vec4& atten, glm::vec4& spot) const;

	bool checkIntersection(const Renderer* renderer) const;

protected:
	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	type m_type;
	glm::vec4 m_color;
	float m_intensity;
	float m_range;
	float m_spotAngle;
	float m_spotFalloff;
	int m_priority;

	static json_interpreter<Light> s_properties;
	static keyword_helper<type> s_types;
};

#endif // LIGHT_HPP