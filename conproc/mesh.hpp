#ifndef MESH_HPP
#define MESH_HPP

#include "processor.hpp"

#include "assimp/vector3.h"

#include <unordered_set>
#include <unordered_map>
#include <set>

struct aiScene;
struct aiNode;
struct aiMesh;

class mesh_processor : public processor
{
public:
	explicit mesh_processor(const conproc* parent);

protected:
	virtual void process_impl(const fs::path& file, const nlohmann::json& options) override;

private:
	struct processed_mesh
	{
		std::string name;
		aiVector3D pivot;
		std::set<unsigned int> subMeshes;
	};

	std::string m_globalName;
	const aiScene* m_scene;
	std::unordered_set<unsigned int> m_meshIndices;
	std::unordered_map<std::string, processed_mesh> m_processedMeshes;
	float m_scale;
	std::unordered_set<std::string> m_includedMeshes;
	unsigned int m_dbgIndent;

	void process_node(const aiNode* node, aiVector3D pivot);
	void iterate_meshlist();
	void add_mesh(const std::string& name, unsigned int index, const aiVector3D& pivot);
	void process_mesh(const processed_mesh& mesh);
};

#endif // MESH_HPP