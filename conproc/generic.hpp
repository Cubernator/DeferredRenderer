#ifndef GENERIC_HPP
#define GENERIC_HPP

#include "processor.hpp"

class generic_processor : public processor
{
public:
	explicit generic_processor(const conproc* parent);

protected:
	virtual void process_impl(const fs::path& file, const nlohmann::json& options) override;
};

#endif // GENERIC_HPP