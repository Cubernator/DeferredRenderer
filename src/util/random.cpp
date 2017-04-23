#include "random.hpp"

namespace random
{
	namespace
	{
		default_gen_t g_def_engine;
	}

	default_gen_t& default_gen()
	{
		return g_def_engine;
	}

	void default_seed_random()
	{
		default_gen().seed(std::random_device()());
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
