#ifndef SCENEIMPORTER_HPP
#define SCENEIMPORTER_HPP

#include "typedefs.hpp"

#include <istream>

class Scene;

class SceneImporter
{
public:
	std::unique_ptr<Scene> fromFile(const path& p);
	std::unique_ptr<Scene> fromStream(std::istream& s);
};

#endif // SCENEIMPORTER_HPP