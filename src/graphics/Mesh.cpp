#include "graphics/Mesh.hpp"
#include "graphics/gl_types.hpp"
#include "util/type_registry.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"

#include <numeric>

REGISTER_OBJECT_TYPE(Mesh, "mesh", ".fbx");

Mesh::Mesh() : m_vao(0) { }

Mesh::Mesh(Mesh&& other)
	: m_vao(other.m_vao),
	m_positions(std::move(other.m_positions)),
	m_normals(std::move(other.m_normals)),
	m_tangents(std::move(other.m_tangents)),
	m_uvs(std::move(other.m_uvs)),
	m_indices(std::move(other.m_indices))
{
	other.m_vao = 0;
}

Mesh& Mesh::operator=(Mesh&& other)
{
	if (this != &other) {
		m_vao = other.m_vao;
		m_positions = std::move(other.m_positions);
		m_normals = std::move(other.m_normals);
		m_tangents = std::move(other.m_tangents);
		m_uvs = std::move(other.m_uvs);
		m_indices = std::move(other.m_indices);
		other.m_vao = 0;
	}

	return *this;
}

Mesh::~Mesh()
{
	if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

void Mesh::bindVAO() const
{
	glBindVertexArray(m_vao);
}

void Mesh::unbindVAO() const
{
	glBindVertexArray(0);
}

void Mesh::bindIndices() const
{
	m_indices->bind();
}

void Mesh::unbindIndices() const
{
	m_indices->unbind();
}

void Mesh::updateVAO()
{
	if (!m_vao) {
		glCreateVertexArrays(1, &m_vao);
	}

	bindVAO();
	setAttribArray(0, 3, m_positions);
	setAttribArray(1, 3, m_normals);
	setAttribArray(2, 3, m_tangents);
	setAttribArray(3, 2, m_uvs);
	unbindVAO();
}

void Mesh::setVertices(std::size_t count, const position_type* positions, const normal_type* normals, const tangent_type* tangents, const uv_type* uvs)
{
	fillBuffer(count, m_positions, positions);
	fillBuffer(count, m_normals, normals);
	fillBuffer(count, m_tangents, tangents);
	fillBuffer(count, m_uvs, uvs);

	updateVAO();
}

void Mesh::setIndices(std::size_t count, const index_type* indices)
{
	fillBuffer(count, m_indices, indices);

	updateBounds();
}

void Mesh::draw()
{
	glDrawElements(GL_TRIANGLES, m_indices->count(), gl_type<index_type>(), nullptr);
}

aabb Mesh::computeBounds() const
{
	glm::vec3 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min());

	const index_type* indices = m_indices->mapReadOnly();
	const position_type* vertices = m_positions->mapReadOnly();

	std::size_t numIndices = m_indices->count();
	for (std::size_t i = 0; i < numIndices; ++i) {
		position_type pos = vertices[indices[i]];

		if (pos.x < min.x) min.x = pos.x;
		if (pos.y < min.y) min.y = pos.y;
		if (pos.z < min.z) min.z = pos.z;

		if (pos.x > max.x) max.x = pos.x;
		if (pos.y > max.y) max.y = pos.y;
		if (pos.z > max.z) max.z = pos.z;
	}

	m_positions->unmap();
	m_indices->unmap();

	return aabb{ min, max };
}

void Mesh::updateBounds()
{
	m_bounds = computeBounds();
}

template<>
std::unique_ptr<Mesh> import_object(const path& filename)
{
	using namespace Assimp;

	unsigned int importFlags = aiProcessPreset_TargetRealtime_MaxQuality;

	Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	const aiScene* scene = importer.ReadFile(filename.string(), importFlags);

	if (scene && scene->mNumMeshes > 0) {
		if (scene->mNumMeshes > 1) {
			std::cout << "WARNING: only one mesh per file can be imported! (" << filename << ")" << std::endl;
		}

		const aiMesh* mesh = scene->mMeshes[0];

		auto positions = reinterpret_cast<const Mesh::position_type*>(mesh->mVertices);
		auto normals = reinterpret_cast<const Mesh::normal_type*>(mesh->mNormals);
		auto tangents = reinterpret_cast<const Mesh::tangent_type*>(mesh->mTangents);

		std::vector<Mesh::uv_type> uvData;
		if (mesh->HasTextureCoords(0)) {
			for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
				uvData.emplace_back(
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				);
			}
		}

		const Mesh::uv_type* uvs = uvData.empty() ? nullptr : uvData.data();

		std::vector<Mesh::index_type> indices;
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			auto& face = mesh->mFaces[f];
			if (face.mNumIndices != 3) continue; // only triangles!
			indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
		}

		auto newMesh = std::make_unique<Mesh>();

		newMesh->setVertices(mesh->mNumVertices, positions, normals, tangents, uvs);
		newMesh->setIndices(indices.size(), indices.data());

		return newMesh;
	}

	return std::unique_ptr<Mesh>();
}
