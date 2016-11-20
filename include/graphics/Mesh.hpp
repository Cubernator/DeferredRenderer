#ifndef MESH_HPP
#define MESH_HPP

#include <memory>

#include "graphics/Buffer.hpp"
#include "util/import.hpp"
#include "util/bounds.hpp"

#include "glm.hpp"

class Mesh
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


	Mesh();
	~Mesh();

	Mesh(Mesh&& other);
	Mesh& operator=(Mesh&& other);

	void bindVAO() const;
	void unbindVAO() const;

	void bindIndices() const;
	void unbindIndices() const;

	void updateVAO();

	void setVertices(std::size_t count,
		const position_type* positions,
		const normal_type* normals,
		const tangent_type* tangents,
		const uv_type* uvs);

	void setIndices(std::size_t count, const index_type* indices);

	void draw();

	const aabb& getBounds() const { return m_bounds; }

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
	void fillBuffer(std::size_t count, std::unique_ptr<T>& buffer, const typename T::element_type* data)
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

template<>
std::unique_ptr<Mesh> import_object<Mesh>(const path& filename);

#endif // MESH_HPP