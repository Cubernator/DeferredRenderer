#ifndef LIGHTINGTEST_HPP
#define LIGHTINGTEST_HPP

#include "core/Component.hpp"
#include "util/bounds.hpp"
#include "util/json_interpreter.hpp"
#include "core/Engine.hpp"

#include <vector>

class ImageEffect;

class LightingTest : public Component
{
public:
	explicit LightingTest(Entity* parent);

	void addLights(unsigned int num);
	void setLightsEnabled(bool val);
	void setLightsMoving(bool val);
	void startTest(bool lights);
	void saveResults();

protected:
	virtual void start_impl() override;
	virtual void update_impl() override;

	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	bool m_lightsEnabled, m_lightsMoving;
	std::vector<Entity*> m_lights;

	unsigned int m_lightCount;
	aabb m_lightArea;
	float m_minSaturation, m_maxSaturation;
	float m_minIntensity, m_maxIntensity;
	float m_rangeFactor;
	float m_minSpeed, m_maxSpeed;

	time_type m_testDuration, m_testTimer;
	time_type m_accFrameTime;
	unsigned int m_testLight, m_testSamples;
	bool m_testing, m_testLights;

	std::vector<time_type> m_ftResults, m_fpsResults;
	std::vector<float> m_lightsPerObj;

	static json_interpreter<LightingTest> s_properties;

	friend class json_interpreter<LightingTest>;
};

#endif // LIGHTINGTEST_HPP