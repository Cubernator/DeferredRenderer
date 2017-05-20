#ifndef IGNORE_HPP
#define IGNORE_HPP

#include "processor.hpp"

class ignore_processor : public processor
{
public:
	explicit ignore_processor(const conproc* parent) : processor(parent) { }

protected:
	virtual void process_impl(const fs::path& file, const nlohmann::json& options) override { }
};

#endif // IGNORE_HPP