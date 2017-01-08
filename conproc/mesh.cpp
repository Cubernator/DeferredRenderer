#include "mesh.hpp"
#include "conproc.hpp"

#include "boost/format.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"

#include <ios>

mesh_processor::mesh_processor(const conproc* parent) : processor(parent), m_scale(1.0f), m_dbgIndent(0) { }

void mesh_processor::process_impl(const fs::path& file, const nlohmann::json& options)
{
	m_globalName = file.stem().string();

	using namespace Assimp;

	auto it = options.find("scale");
	if (it != options.end() && it->is_number()) {
		m_scale = *it;
	}

	it = options.find("includedMeshes");
	if (it != options.end() && it->is_array()) {
		std::vector<std::string> im = *it;
		m_includedMeshes.insert(im.begin(), im.end());
	}

	unsigned int importFlags = aiProcessPreset_TargetRealtime_Quality;

	Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	m_scene = importer.ReadFile(file.string(), importFlags);

	if (!m_scene) {
		debug_output() << "ERROR: failed to process scene: " << importer.GetErrorString() << std::endl;
		return;
	}

	debug_output() << "traversing scene nodes..." << std::endl;
	process_node(m_scene->mRootNode, aiVector3D());
	debug_output() << "...found " << m_processedMeshes.size() << " meshes." << std::endl;

	if (m_processedMeshes.empty()) {
		debug_output() << "iterating mesh list..." << std::endl;
		iterate_meshlist();
		debug_output() << "...done." << std::endl;
	}

	debug_output() << "processing meshes..." << std::endl;
	for (auto& p : m_processedMeshes) {
		process_mesh(p.second);
	}
	debug_output() << "...done." << std::endl;
}

void mesh_processor::process_node(const aiNode* node, aiVector3D pivot)
{
	std::string indent;
	for (unsigned int i = 0; i < m_dbgIndent; ++i) indent += "  ";

	std::string nodeName{ node->mName.C_Str() };

	debug_output() << indent << "node " << nodeName << " referencing " << node->mNumMeshes << " meshes" << std::endl;

	// test if this node is a helper node generated by assimp which contains information about the object's pivot point
	auto ps = nodeName.find("$AssimpFbx$_RotationPivot");
	if (ps != std::string::npos) {
		// for every pivot node assimp also generates an "inverse" node, which is called the same but with "Inverse" at the end.
		// we want to avoid these, since they are the exact reverse
		ps = nodeName.find("Inverse", ps);
		if (ps == std::string::npos) {
			aiVector3D p;
			aiQuaternion r;
			node->mTransformation.DecomposeNoScaling(r, p);
			pivot += p;
		}
	}

	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		unsigned int m = node->mMeshes[i];
		auto p = m_meshIndices.insert(m);
		if (p.second) {
			const aiMesh* mesh = m_scene->mMeshes[m];
			std::string meshName{ mesh->mName.C_Str() };
			if (meshName.empty())
				meshName = nodeName;

			debug_output() << indent << " new mesh: " << meshName << std::endl;
			add_mesh(meshName, m, pivot);
		}
	}

	++m_dbgIndent;
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		process_node(node->mChildren[i], pivot);
	}
	--m_dbgIndent;
}

void mesh_processor::iterate_meshlist()
{
	debug_output() << "this file contains " << m_scene->mNumMeshes << " meshes." << std::endl;

	for (unsigned int i = 0; i < m_scene->mNumMeshes; ++i) {
		const aiMesh* mesh = m_scene->mMeshes[i];
		std::string meshName{ mesh->mName.C_Str() };
		if (meshName.empty())
			meshName = m_globalName;

		debug_output() << "  found mesh: " << meshName << std::endl;
		add_mesh(meshName, i, aiVector3D());
	}
}

void mesh_processor::add_mesh(const std::string& name, unsigned int index, const aiVector3D& pivot)
{
	auto it = m_processedMeshes.find(name);
	if (it != m_processedMeshes.end()) {
		it->second.subMeshes.insert(index);
	} else {
		m_processedMeshes.emplace(name, processed_mesh{ name, pivot, { index } });
	}
}

void mesh_processor::process_mesh(const processed_mesh& mesh)
{
	if (!m_includedMeshes.count(mesh.name)) {
		debug_output() << "mesh " << mesh.name << " won't be processed: not in included mesh list!" << std::endl;
		return;
	}

	fs::path fileName = m_parent->dest_dir() / mesh.name;
	fileName += ".rbm";

	debug_output() << "processing mesh " << mesh.name << " into file: " << fileName << std::endl;

	fs::ofstream output(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
	if (output) {
		unsigned int subMeshCount = mesh.subMeshes.size();
		output.write(reinterpret_cast<char*>(&subMeshCount), sizeof(subMeshCount));

		unsigned int sm = 0;
		for (unsigned int index : mesh.subMeshes) {
			const aiMesh* subMesh = m_scene->mMeshes[index];

			auto vertexData = subMesh->mVertices;
			auto normalData = subMesh->mNormals;
			auto tangentData = subMesh->mTangents;
			aiVector2D* uvData = nullptr;

			unsigned int vertexCount = subMesh->mNumVertices;

			std::vector<aiVector3D> vertices;
			if (m_scale != 1.0f) {
				vertices.resize(vertexCount);
				for (unsigned int i = 0; i < vertexCount; ++i) {
					// offset all vertices by the node's pivot point (mainly used when importing FBX files)
					// also apply global scaling factor
					vertices[i] = (subMesh->mVertices[i] - mesh.pivot) * m_scale;
				}
				vertexData = vertices.data();
			}

			std::vector<aiVector2D> uvs;
			if (subMesh->HasTextureCoords(0)) {
				uvs.resize(vertexCount);
				for (unsigned int i = 0; i < vertexCount; ++i) {
					uvs[i].x = subMesh->mTextureCoords[0][i].x;
					uvs[i].y = 1.f - subMesh->mTextureCoords[0][i].y; // flip v-coordinate because opengl stores textures "upside down"
				}
				uvData = uvs.data();
			}

			unsigned char compMask = 0x00u;
			if (vertexData) compMask |= 0x80u;
			if (normalData) compMask |= 0x40u;
			if (tangentData) compMask |= 0x20u;
			if (uvData) compMask |= 0x10u;

			std::vector<unsigned int> indices;
			for (unsigned int f = 0; f < subMesh->mNumFaces; ++f) {
				auto& face = subMesh->mFaces[f];
				if (face.mNumIndices != 3) continue; // only triangles!
				indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
			}

			unsigned int indexCount = indices.size();
			auto indexData = indices.data();

			debug_output() << "  submesh " << sm << ": " << vertexCount << " vertices, " << indexCount << " indices" << std::endl;

			output.write(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
			output.write(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

			output.write(reinterpret_cast<char*>(&compMask), sizeof(compMask));

			if (vertexData) output.write(reinterpret_cast<char*>(vertexData), sizeof(vertexData[0]) * vertexCount);
			if (normalData) output.write(reinterpret_cast<char*>(normalData), sizeof(normalData[0]) * vertexCount);
			if (tangentData) output.write(reinterpret_cast<char*>(tangentData), sizeof(tangentData[0]) * vertexCount);
			if (uvData) output.write(reinterpret_cast<char*>(uvData), sizeof(uvData[0]) * vertexCount);

			output.write(reinterpret_cast<char*>(indexData), sizeof(indexData[0]) * indexCount);

			++sm;
		}
	} else {
		debug_output() << "ERROR: failed to open file!" << std::endl;
	}
}