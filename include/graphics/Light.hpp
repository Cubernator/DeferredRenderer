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
		type_directional,
		type_point,
		type_spot
	};

	Light(Entity* parent);

	type getType() const { return m_type; }
	const glm::vec4& getColor() const { return m_color; }
	float getIntensity() const { return m_intensity; }
	float getRange() const { return m_range; }
	int getPriority() const { return m_priority; }

	void setType(type val) { m_type = val; }
	void setColor(const glm::vec4& val) { m_color = val; }
	void setIntensity(float val) { m_intensity = val; }
	void setRange(float val) { m_range = val; }
	void setPriority(int val) { m_priority = val; }

	bool isVisible() const;

	glm::vec4 getShaderProp() const;
	glm::vec4 getPremultipliedColor() const;

	bool checkIntersection(const Renderer* renderer) const;

protected:
	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	type m_type;
	glm::vec4 m_color;
	float m_intensity;
	float m_range;
	int m_priority;

	static json_interpreter<Light> s_properties;
	static keyword_helper<type> s_types;

	void extractType(const nlohmann::json& json);
	void extractColor(const nlohmann::json& json);
	void extractIntensity(const nlohmann::json& json);
	void extractRange(const nlohmann::json& json);
	void extractPriority(const nlohmann::json& json);
};

#endif // LIGHT_HPP