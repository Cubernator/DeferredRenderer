#include "Mesh.hpp"
#include "gl_types.hpp"
#include "core/type_registry.hpp"
#include "scripting/class_registry.hpp"

#include <numeric>

namespace hexeract
{
	namespace graphics
	{
		REGISTER_OBJECT_TYPE(Mesh, ".rbm");

		SubMesh::SubMesh() : m_vao(0) { }

		SubMesh::SubMesh(SubMesh&& other)
			: m_vao(other.m_vao),
			m_positions(std::move(other.m_positions)),
			m_normals(std::move(other.m_normals)),
			m_tangents(std::move(other.m_tangents)),
			m_uvs(std::move(other.m_uvs)),
			m_indices(std::move(other.m_indices))
		{
			other.m_vao = 0;
		}

		SubMesh& SubMesh::operator=(SubMesh&& other)
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

		SubMesh::~SubMesh()
		{
			if (m_vao) glDeleteVertexArrays(1, &m_vao);
		}

		void SubMesh::bindVAO() const
		{
			glBindVertexArray(m_vao);
		}

		void SubMesh::unbindVAO() const
		{
			glBindVertexArray(0);
		}

		void SubMesh::bindIndices() const
		{
			m_indices->bind();
		}

		void SubMesh::unbindIndices() const
		{
			m_indices->unbind();
		}

		void SubMesh::updateVAO()
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

		void SubMesh::setVertices(size_type count, const position_type* positions, const normal_type* normals, const tangent_type* tangents, const uv_type* uvs)
		{
			fillBuffer(count, m_positions, positions);
			fillBuffer(count, m_normals, normals);
			fillBuffer(count, m_tangents, tangents);
			fillBuffer(count, m_uvs, uvs);

			updateVAO();
		}

		void SubMesh::setIndices(size_type count, const index_type* indices)
		{
			fillBuffer(count, m_indices, indices);

			updateBounds();
		}

		void SubMesh::bind() const
		{
			bindVAO();
			bindIndices();
		}

		void SubMesh::unbind() const
		{
			unbindVAO();
			unbindIndices();
		}

		void SubMesh::draw() const
		{
			glDrawElements(GL_TRIANGLES, GLsizei(m_indices->count()), gl_type<index_type>(), nullptr);
		}

		std::size_t SubMesh::triangles() const
		{
			return m_indices->count() / 3;
		}

		aabb SubMesh::computeBounds() const
		{
			glm::vec3 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::lowest());

			const index_type* indices = m_indices->mapReadOnly();
			const position_type* vertices = m_positions->mapReadOnly();

			size_type numIndices = m_indices->count();
			for (size_type i = 0; i < numIndices; ++i) {
				position_type pos = vertices[indices[i]];

				min = glm::min(min, pos);
				max = glm::max(max, pos);
			}

			m_positions->unmap();
			m_indices->unmap();

			return aabb{ min, max };
		}

		void SubMesh::updateBounds()
		{
			m_bounds = computeBounds();
		}

		void Mesh::addSubMesh(std::unique_ptr<SubMesh> subMesh)
		{
			m_bounds = aabb_union(m_bounds, subMesh->bounds());
			m_subMeshes.push_back(std::move(subMesh));
		}

		void Mesh::clearSubMeshes()
		{
			m_subMeshes.clear();
			m_bounds = aabb();
		}

		SCRIPTING_REGISTER_DERIVED_CLASS(SubMesh, Object);

		SCRIPTING_AUTO_METHOD(SubMesh, bounds);
		SCRIPTING_AUTO_METHOD(SubMesh, triangles);

		SCRIPTING_REGISTER_DERIVED_CLASS(Mesh, NamedObject);

		SCRIPTING_AUTO_METHOD(Mesh, subMeshCount);
		SCRIPTING_AUTO_METHOD(Mesh, getSubMesh);
		SCRIPTING_AUTO_METHOD(Mesh, bounds);
	}


	template<>
	std::unique_ptr<graphics::Mesh> import_object(const path& filename)
	{
		using namespace graphics;

		boost::filesystem::ifstream file(filename, std::ios::binary);
		if (file) {
			auto newMesh = std::make_unique<Mesh>();

			unsigned int subMeshCount;
			file.read(reinterpret_cast<char*>(&subMeshCount), sizeof(subMeshCount));

			for (unsigned int i = 0; i < subMeshCount; ++i) {
				unsigned int vertexCount, indexCount;
				unsigned char compMask;

				file.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
				file.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

				file.read(reinterpret_cast<char*>(&compMask), sizeof(compMask));

				bool hasVertices = (compMask & 0x80u) == 0x80u;
				bool hasNormals = (compMask & 0x40u) == 0x40u;
				bool hasTangents = (compMask & 0x20u) == 0x20u;
				bool hasUvs = (compMask & 0x10u) == 0x10u;

				std::vector<SubMesh::position_type> vertices;
				std::vector<SubMesh::normal_type> normals;
				std::vector<SubMesh::tangent_type> tangents;
				std::vector<SubMesh::uv_type> uvs;

				if (hasVertices) {
					vertices.resize(vertexCount);
					file.read(reinterpret_cast<char*>(vertices.data()), sizeof(SubMesh::position_type) * vertexCount);
				}
				if (hasNormals) {
					normals.resize(vertexCount);
					file.read(reinterpret_cast<char*>(normals.data()), sizeof(SubMesh::normal_type) * vertexCount);
				}
				if (hasTangents) {
					tangents.resize(vertexCount);
					file.read(reinterpret_cast<char*>(tangents.data()), sizeof(SubMesh::tangent_type) * vertexCount);
				}
				if (hasUvs) {
					uvs.resize(vertexCount);
					file.read(reinterpret_cast<char*>(uvs.data()), sizeof(SubMesh::uv_type) * vertexCount);
				}

				auto vertexData = hasVertices ? vertices.data() : nullptr;
				auto normalData = hasNormals ? normals.data() : nullptr;
				auto tangentData = hasTangents ? tangents.data() : nullptr;
				auto uvData = hasUvs ? uvs.data() : nullptr;

				std::vector<SubMesh::index_type> indices(indexCount);
				file.read(reinterpret_cast<char*>(indices.data()), sizeof(SubMesh::index_type) * indexCount);

				auto subMesh = std::make_unique<SubMesh>();
				subMesh->setVertices(vertexCount, vertexData, normalData, tangentData, uvData);
				subMesh->setIndices(indexCount, indices.data());
				newMesh->addSubMesh(std::move(subMesh));
			}

			return std::move(newMesh);
		}

		return std::unique_ptr<Mesh>();
	}
}
