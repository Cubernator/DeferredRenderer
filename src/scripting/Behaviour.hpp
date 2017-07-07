#ifndef BEHAVIOUR_HPP
#define BEHAVIOUR_HPP

#include "core\Component.hpp"

#include "utility.hpp"

#include <exception>

namespace scripting
{
	class Behaviour : public Component
	{
	public:
		explicit Behaviour(Entity* parent);
		virtual ~Behaviour();

		const std::string& script() const { return m_script; }
		void setScript(const std::string& name);

		template<typename T>
		T getProperty(const std::string& name)
		{
			if (!m_good) {
				throw std::runtime_error("Cannot get property of bad Behaviour!");
			}

			// TODO
		}

		template<typename T>
		void setProperty(const std::string& name, T&& value)
		{
			if (!m_good) {
				throw std::runtime_error("Cannot set property on bad Behaviour!");
			}

			lua_State* L = getState();

			beginProperties(L);
			push_value(L, std::forward<T>(value));
			submitProperty(L, name);
			endProperties(L);
		}

		bool isGood() const { return m_good; }

		COMPONENT_ALLOW_MULTIPLE;

	protected:
		virtual void apply_json_impl(const nlohmann::json& json) override;

	private:
		std::string m_script;
		bool m_good;
		int m_objIdx;

		void destroyObj();

		void beginProperties(lua_State* L);
		void endProperties(lua_State* L);
		void submitProperty(lua_State* L, const std::string& name);

		static lua_State* getState();
	};
}

#endif // BEHAVIOUR_HPP