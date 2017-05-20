#ifndef DAYNIGHTCONTROLLER_HPP
#define DAYNIGHTCONTROLLER_HPP

#include "core/Component.hpp"
#include "util/json_interpreter.hpp"

class Light;

class DayNightController : public Component
{
public:
	explicit DayNightController(Entity* parent);

	bool getDayTime() const { return m_dayTime; }
	void setDayTime(bool val);

	bool getAmbient() const { return m_ambient; }
	void setAmbient(bool val);

protected:
	virtual void start_impl() override;
	virtual void update_impl() override;

	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json) override;

private:
	bool m_dayTime, m_ambient;
	Light* m_light;

	glm::vec4 m_daySkyColor, m_nightSkyColor;
	glm::vec4 m_dayLightColor, m_nightLightColor;
	float m_dayLightIntensity, m_nightLightIntensity;

	static json_interpreter<DayNightController> s_properties;

	friend class json_interpreter<DayNightController>;

	void applyValues();
};

#endif // DAYNIGHTCONTROLLER_HPP