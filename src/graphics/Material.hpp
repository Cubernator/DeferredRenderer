#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "core/NamedObject.hpp"
#include "util/import.hpp"
#include "util/json_initializable.hpp"
#include "graphics/shader/shader_property.hpp"

class Effect;
class ShaderProgram;

class Material : public NamedObject, public json_initializable<Material>
{
public:
	Material();

	Effect* effect() { return m_effect; }
	const Effect* effect() const { return m_effect; }
	void setEffect(Effect* effect);

	const shader_property* getProperty(uniform_id id) const;

	template<typename T>
	bool getPropertyValue(uniform_id id, T& value) const
	{
		const shader_property* prop = getProperty(id);
		if (prop) {
			value = prop->value;
			return true;
		}
		return false;
	}

	template<typename T>
	T getPropertyValue(uniform_id id) const
	{
		T tmp;
		getProperty(id, tmp);
		return std::move(tmp);
	}

	template<typename T>
	void setPropertyValue(uniform_id id, const T& newValue)
	{
		auto it = m_properties.find(id);
		if (it != m_properties.end()) {
			m_properties.modify(it, [&json](shader_property& prop) {
				prop.set(newValue);
			});
		}
	}

	void apply(const ShaderProgram* p) const;

private:
	Effect* m_effect;
	shader_property_map m_properties;

	// cppcheck-suppress unusedPrivateFunction
	void apply_json_impl(const nlohmann::json& json);

	void setPropFromJson(const std::string& name, const nlohmann::json& json);

	friend struct json_initializable<Material>;
};

#endif // MATERIAL_HPP