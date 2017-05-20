#ifndef DEBUGCONTROLS_HPP
#define DEBUGCONTROLS_HPP

#include "core/Component.hpp"

#include <vector>

class ImageEffect;

class DebugControls : public Component
{
public:
	explicit DebugControls(Entity* parent);

	void setImageEffectsEnabled(bool val);

protected:
	virtual void start_impl() override;
	virtual void update_impl() override;

private:
	bool m_imgEffectsEnabled;
	std::vector<ImageEffect*> m_imgEffects;

	static std::string getOutputName(int o);
};

#endif // DEBUGCONTROLS_HPP