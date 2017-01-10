#include "random.hpp"

namespace random
{
	namespace
	{
		std::default_random_engine g_def_engine{ std::random_device()() };
	}

	std::default_random_engine& default_gen()
	{
		return g_def_engine;
	}

	glm::vec3 uniform_direction()
	{
		std::normal_distribution<float> dist{ 0.0f, 1.0f };
		auto& gen = default_gen();

		// if each coordinate is normally distributed then the normalized vector is uniformly distributed on the surface of the unit sphere
		glm::vec3 result{ dist(gen), dist(gen), dist(gen) };
		float l2 = glm::max(0.001f, glm::length2(result));
		return result * glm::inversesqrt(l2);
	}
}
