#ifndef LIGHTSWITCH_HPP
#define LIGHTSWITCH_HPP

#include "core/Component.hpp"
#include "util/json_interpreter.hpp"
#include "input/keys.hpp"

class Light;

class LightSwitch : public Component
{
public:
	explicit LightSwitch(Entity* parent);

protected:
	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json);

	virtual void start_impl() override;
	virtual void update_impl() override;

private:
	Light* m_light;
	input_key m_key;

	friend class json_interpreter<LightSwitch>;

	static json_interpreter<LightSwitch> s_properties;
};

#endif // LIGHTSWITCH_HPP