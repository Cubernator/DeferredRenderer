#ifndef LIGHTINGTEST_HPP
#define LIGHTINGTEST_HPP

#include "core/Component.hpp"
#include "util/bounds.hpp"
#include "util/json_interpreter.hpp"

#include <vector>

class LightingTest : public Component
{
public:
	LightingTest(Entity* parent);

	void setLightsEnabled(bool val);

protected:
	virtual void start_impl() override;
	virtual void update_impl() override;

	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	bool m_lightsEnabled;
	std::vector<Entity*> m_lights;

	unsigned int m_lightCount;
	aabb m_lightArea;

	static json_interpreter<LightingTest> s_properties;

	friend class json_interpreter<LightingTest>;
};

#endif // LIGHTINGTEST_HPP