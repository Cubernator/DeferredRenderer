#include "json_utils.hpp"

#include <sstream>

glm::vec4 parse_color(const std::string& s)
{
	glm::vec4 color(0.f);

	int maxSize = ((int)s.size() - 1) / 2;

	for (int i = 0; i < maxSize; i++) {
		std::stringstream ss;
		ss << std::hex << s.substr(i * 2 + 1, 2);
		int cv;
		ss >> cv;
		color[i] = cv / 255.0f;
	}

	if (maxSize < 4) // assume full opacity if no alpha value is specified
		color.a = 1.0f;

	// assume all input color values are in sRGB space
	return glm::convertSRGBToLinear(color);
}
