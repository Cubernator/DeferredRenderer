#ifndef SCRIPTINGENVIRONMENT_HPP
#define SCRIPTINGENVIRONMENT_HPP

#include "path.hpp"
#include "guid.hpp"

#include "util/singleton.hpp"
#include "core/ComponentModule.hpp"

#include "utility.hpp"

#include <string>
#include <vector>

class Entity;

namespace scripting
{
	class Behaviour;

	class Environment : public singleton<Environment>, public ComponentModule
	{
	public:
		Environment();
		~Environment();

		void update();

		lua_State *state() { return m_L; }

		void stackDump()
		{
			stack_dump(m_L);
		}

		void pop(int n = 1)
		{
			lua_pop(m_L, n);
		}

		template<typename T>
		void pushValue(T&& value)
		{
			push_value(m_L, std::forward<T>(value));
		}

		template<typename T>
		T getValue(int idx = -1)
		{
			return get_value<T>(m_L, idx);
		}

		bool loadModule(const std::string& name);

		void addClass(const std::string& name, const std::string& base);
		void addClass(const std::string& name);
		void addStaticClass(const std::string& name);

		template<typename Iter>
		void addMethods(const std::string& className, Iter beginMethods, Iter endMethods)
		{
			lua_getglobal(m_L, className.c_str());

			for (Iter it = beginMethods; it != endMethods; ++it) {
				lua_pushcfunction(m_L, it->second);
				set_field(m_L, -2, it->first);
			}

			pop();
		}

		void addMethods(const std::string& className, std::initializer_list<std::pair<std::string, lua_CFunction>> methods)
		{
			addMethods(className, methods.begin(), methods.end());
		}

		void addMethod(const std::string& className, const std::string& name, lua_CFunction f)
		{
			addMethods(className, { { name, f } });
		}

		void instantiateClass(const std::string& className, Object* obj);
		void createObject(const std::string& className, Object* obj, bool pop = true)
		{
			instantiateClass(className, obj);
			if (pop) {
				this->pop();
			}
		}

		void invalidateObject(Object* obj);
		void pushObject(Object* obj);

		int safeCall(int argc, int resc = LUA_MULTRET);

		template<typename... Args>
		void callMethod(Object* obj, const std::string& methodName, int numResults, Args&&... args)
		{
			pushObject(obj);

			if (lua_istable(m_L, -1)) {
				lua_getfield(m_L, -1, methodName.c_str());

				if (lua_isfunction(m_L, -1)) {
					swap_top_2(m_L);
					push_values(m_L, std::forward<Args>(args)...);
					int ac = 1 + sizeof...(args);
					safeCall(ac, numResults);
				} else {
					std::cout << "ERROR: could not find method: \"" << methodName << "\"!" << std::endl;
					pop(2);
				}
			} else {
				std::cout << "ERROR: could not find object!" << std::endl;
				pop();
			}
		}

		void callBehaviourMethod(Behaviour* bhv, const std::string& methodName);

		virtual void addComponent(Component* cmpt) final;
		virtual void removeComponent(Component* cmpt) final;

	private:
		lua_State *m_L;
		std::vector<Behaviour*> m_behaviours, m_addBhvs, m_removeBhvs;

		static int traceback(lua_State* L);
		static int onPanic(lua_State* L);
	};
}

#endif // SCRIPTINGENVIRONMENT_HPP