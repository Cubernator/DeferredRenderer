#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>
#include <functional>

#include "glm.hpp"

namespace random
{
	using default_gen_t = std::default_random_engine;

	default_gen_t& default_gen();

	inline void default_seed(default_gen_t::result_type value = default_gen_t::default_seed)
	{
		default_gen().seed(value);
	}

	void default_seed_random();

	template<typename DistType, typename GenType>
	std::function<typename DistType::result_type()> get_function(DistType&& dist, GenType& gen)
	{
		return std::bind(std::forward<DistType>(dist), std::ref(gen));
	}

	template<typename DistType>
	std::function<typename DistType::result_type()> get_function_def(DistType&& dist)
	{
		return get_function(std::forward<DistType>(dist), default_gen());
	}

	template<typename DistType, typename GenType, typename ...Args>
	std::function<typename DistType::result_type()> make_function(GenType& gen, Args&&... args)
	{
		return get_function(DistType{ std::forward<Args>(args)... }, gen);
	}

	template<typename DistType, typename ...Args>
	std::function<typename DistType::result_type()> make_function_def(Args&&... args)
	{
		return get_function_def(DistType{ std::forward<Args>(args)... });
	}

	template<typename RealType>
	RealType uniform_real(RealType min, RealType max)
	{
		return std::uniform_real_distribution<RealType>{min, max}(default_gen());
	}

	inline float uniform_float(float min, float max)
	{
		return uniform_real<float>(min, max);
	}

	inline float uniform_float(float max = 1.0f)
	{
		return uniform_real<float>(0.0f, max);
	}

	inline glm::vec3 uniform_vec3(const glm::vec3& min, const glm::vec3& max)
	{
		return{
			uniform_float(min.x, max.x),
			uniform_float(min.y, max.y),
			uniform_float(min.z, max.z)
		};
	}

	inline glm::vec3 uniform_vec3(const glm::vec3& max)
	{
		return uniform_vec3(glm::vec3(0.f), max);
	}

	template<typename RealType>
	RealType normal_real(RealType mean, RealType stddev)
	{
		std::normal_distribution<RealType>{mean, stddev}(default_gen());
	}

	inline float normal_float(float mean = 0.0f, float stddev = 1.0f)
	{
		return normal_real<float>(mean, stddev);
	}

	glm::vec3 uniform_direction();
}

#endif // RANDOM_HPP