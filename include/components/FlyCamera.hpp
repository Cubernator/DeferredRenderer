#ifndef FLYCAMERA_HPP
#define FLYCAMERA_HPP

#include "core/Component.hpp"
#include "util/component_registry.hpp"
#include "util/json_interpreter.hpp"

class FlyCamera : public Component
{
public:
	FlyCamera(Entity* parent);

	COMPONENT_DISALLOW_MULTIPLE;

protected:
	virtual void apply_json_property_impl(const std::string& name, const nlohmann::json& json);
	virtual void update_impl() override;

private:
	float m_normalSpeed, m_fastSpeed;
	float m_sensitivityX, m_sensitivityY;
	float m_angleX, m_angleY;

	static json_interpreter<FlyCamera> s_properties;

	void extractNormalSpeed(const nlohmann::json& json);
	void extractFastSpeed(const nlohmann::json& json);
	void extractSensitivityX(const nlohmann::json& json);
	void extractSensitivityY(const nlohmann::json& json);

	REGISTER_COMPONENT_CLASS_DECL(FlyCamera);
};

#endif // FLYCAMERA_HPP