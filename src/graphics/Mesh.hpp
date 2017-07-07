#ifndef MESH_HPP
#define MESH_HPP

#include "Buffer.hpp"
#include "Drawable.hpp"
#include "core/NamedObject.hpp"
#include "util/import.hpp"
#include "util/bounds.hpp"

#include "glm.hpp"

#include <memory>

class SubMesh : public Object, public Drawable
{
public:
	using position_type			= glm::vec3;
	using normal_type			= glm::vec3;
	using tangent_type			= glm::vec3;
	using uv_type				= glm::vec2;

	using index_type			= uint32_t;

	using position_buffer_type	= VertexBuffer<position_type>;
	using normal_buffer_type	= VertexBuffer<normal_type>;
	using tangent_buffer_type	= VertexBuffer<tangent_type>;
	using uv_buffer_type		= VertexBuffer<uv_type>;

	using index_buffer_type		= IndexBuffer<index_type>;

	using size_type				= index_buffer_type::size_type;


	SubMesh();
	~SubMesh();

	SubMesh(SubMesh&& other);
	SubMesh& operator=(SubMesh&& other);

	void bindVAO() const;
	void unbindVAO() const;

	void bindIndices() const;
	void unbindIndices() const;

	void updateVAO();

	void setVertices(size_type count,
		const position_type* positions,
		const normal_type* normals,
		const tangent_type* tangents,
		const uv_type* uvs);

	void setIndices(size_type count, const index_type* indices);

	virtual void bind() const final;
	virtual void unbind() const final;
	virtual void draw() const final;

	virtual aabb bounds() const final { return m_bounds; }
	virtual std::size_t triangles() const final;

private:
	GLuint m_vao;

	std::unique_ptr<position_buffer_type>	m_positions;
	std::unique_ptr<normal_buffer_type>		m_normals;
	std::unique_ptr<tangent_buffer_type>	m_tangents;
	std::unique_ptr<uv_buffer_type>			m_uvs;

	std::unique_ptr<index_buffer_type>		m_indices;

	aabb m_bounds;

	aabb computeBounds() const;
	void updateBounds();

	template<typename T>
	void fillBuffer(size_type count, std::unique_ptr<T>& buffer, const typename T::element_type* data)
	{
		if (data) {
			if (!buffer) buffer = std::make_unique<T>();
			buffer->setData(count, data);
		} else {
			buffer.reset();
		}
	}

	template<typename T>
	void setAttribArray(GLuint index, GLint components, const std::unique_ptr<T>& buffer)
	{
		if (buffer) {
			glEnableVertexAttribArray(index);
			buffer->bind();
			glVertexAttribPointer(index, components, GL_FLOAT, false, 0, 0);
		} else {
			glDisableVertexAttribArray(index);
		}
	}
};

class Mesh : public NamedObject
{
public:
	std::size_t subMeshCount() const { return m_subMeshes.size(); }

	SubMesh* getSubMesh(std::size_t index) { return m_subMeshes[index].get(); }
	const SubMesh* getSubMesh(std::size_t index) const { return m_subMeshes[index].get(); }

	void addSubMesh(std::unique_ptr<SubMesh> subMesh);
	void clearSubMeshes();

	const aabb& bounds() const { return m_bounds; }

private:
	std::vector<std::unique_ptr<SubMesh>> m_subMeshes;

	aabb m_bounds;
};

template<>
std::unique_ptr<Mesh> import_object<Mesh>(const path& filename);

#endif // MESH_HPP