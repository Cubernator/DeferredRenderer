#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "GL\glew.h"

#include <stdint.h>

template<typename ElementType, GLenum target>
class Buffer
{
public:
	using element_type = ElementType;

	Buffer(GLenum usage = GL_STATIC_DRAW) : m_glObj(0), m_usage(usage), m_count(0)
	{
		glCreateBuffers(1, &m_glObj);
	}

	~Buffer()
	{
		if (m_glObj)
			glDeleteBuffers(1, &m_glObj);
	}

	Buffer(Buffer&& other) : m_glObj(other.m_glObj), m_usage(other.m_usage), m_count(other.m_count)
	{
		other.m_glObj = 0;
		other.m_count = 0;
	}

	Buffer& operator=(Buffer&& other)
	{
		if (this != &other) {
			m_glObj = other.m_glObj;
			m_usage = other.m_usage;
			m_count = other.m_count;

			other.m_glObj = 0;
			other.m_count = 0;
		}

		return *this;
	}

	operator bool() const {
		return getObj();
	}

	GLuint getObj() const { return m_glObj; }
	GLsizeiptr getCount() const { return m_count; }

	void bind()
	{
		glBindBuffer(target, m_glObj);
	}

	void unbind()
	{
		glBindBuffer(target, 0);
	}

	void setData(std::size_t count, const ElementType* data, GLenum usage = 0)
	{
		if (usage) m_usage = usage; // modify usage policy if function argument was set

		bind();
		glBufferData(target, count * sizeof(ElementType), data, m_usage);

		m_count = count;
	}

	void clearData()
	{
		setData(0, nullptr);
	}

private:
	GLuint m_glObj;
	GLenum m_usage;
	std::size_t m_count;
};


template<typename VertexType>
using VertexBuffer = Buffer<VertexType, GL_ARRAY_BUFFER>;

template<typename IndexType>
using IndexBuffer = Buffer<IndexType, GL_ELEMENT_ARRAY_BUFFER>;


template<typename IndexType> constexpr GLenum getGLType() { return 0; }

template<> inline constexpr GLenum getGLType<uint8_t>() { return GL_UNSIGNED_BYTE; }
template<> inline constexpr GLenum getGLType<uint16_t>() { return GL_UNSIGNED_SHORT; }
template<> inline constexpr GLenum getGLType<uint32_t>() { return GL_UNSIGNED_INT; }

template<> inline constexpr GLenum getGLType<int8_t>() { return GL_BYTE; }
template<> inline constexpr GLenum getGLType<int16_t>() { return GL_SHORT; }
template<> inline constexpr GLenum getGLType<int32_t>() { return GL_INT; }

template<> inline constexpr GLenum getGLType<float>() { return GL_FLOAT; }
template<> inline constexpr GLenum getGLType<double>() { return GL_DOUBLE; }

#endif // BUFFER_HPP