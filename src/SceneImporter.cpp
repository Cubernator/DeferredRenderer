#include "SceneImporter.hpp"
#include "Scene.hpp"

#include "json_utils.hpp"

#include <iostream>

using namespace nlohmann;
using namespace json_utils;

std::unique_ptr<Scene> SceneImporter::fromFile(const path & p)
{
	boost::filesystem::ifstream file(p);
	return fromStream(file);
}

std::unique_ptr<Scene> SceneImporter::fromStream(std::istream& s)
{
	auto scene = std::make_unique<Scene>();

	if (s) {
		json j;
		s >> j;

		if (j.is_object() && is_type(j, "scene")) {
			auto bcit = j.find("backColor");
			if (bcit != j.end()) {
				scene->setBackColor(convert_to_color(*bcit));
			}
		}
	}

	return scene;
}
