#ifndef BEHAVIOUR_HPP
#define BEHAVIOUR_HPP

#include "core\Component.hpp"

namespace scripting
{
	class Behaviour : public Component
	{
	public:
		explicit Behaviour(Entity* parent);
		virtual ~Behaviour();

		COMPONENT_ALLOW_MULTIPLE;

	protected:
		virtual void start_impl() override;
		virtual void update_impl() override;

		virtual void apply_json_impl(const nlohmann::json& json) override;

	private:
		std::string m_behaviourName;
	};
}

#endif // BEHAVIOUR_HPP