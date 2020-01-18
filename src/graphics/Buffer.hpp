#ifndef GRAPHICS_BUFFER_HPP
#define GRAPHICS_BUFFER_HPP

#include "GL\glew.h"

namespace hexeract
{
	namespace graphics
	{
		template<typename ElementType, GLenum t>
		class Buffer
		{
		public:
			using size_type = GLsizeiptr;
			using element_type = ElementType;
			static constexpr GLenum target = t;

			explicit Buffer(GLenum usage = GL_STATIC_DRAW) : m_glObj(0), m_usage(usage), m_count(0)
			{
				glCreateBuffers(1, &m_glObj);
			}

			~Buffer()
			{
				if (m_glObj)
					glDeleteBuffers(1, &m_glObj);
			}

			Buffer(const Buffer& other) : Buffer(other.m_usage)
			{
				copyFrom(&other);
			}

			Buffer(Buffer&& other) : m_glObj(other.m_glObj), m_usage(other.m_usage), m_count(other.m_count)
			{
				other.m_glObj = 0;
				other.m_count = 0;
			}

			Buffer& operator=(const Buffer& other)
			{
				if (this != &other) {
					m_usage = other.m_usage;
					copyFrom(&other);
				}

				return *this;
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
				return glObj();
			}

			constexpr GLenum glTarget() const { return target; }
			GLuint glObj() const { return m_glObj; }
			size_type count() const { return m_count; }

			void bind()
			{
				glBindBuffer(target, m_glObj);
			}

			void unbind()
			{
				glBindBuffer(target, 0);
			}

			void setData(size_type count, const element_type* data, GLenum usage = 0)
			{
				if (usage) m_usage = usage; // modify usage policy if function argument was set

				bind();
				glBufferData(target, count * sizeof(element_type), data, m_usage);

				m_count = count;
			}

			void copyFrom(const Buffer* other)
			{
				setData(other->m_count, nullptr);

				glBindBuffer(GL_COPY_READ_BUFFER, other->m_glObj);
				glBindBuffer(GL_COPY_WRITE_BUFFER, m_glObj);

				glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, count * sizeof(element_type));

				glBindBuffer(GL_COPY_READ_BUFFER, 0);
				glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
			}

			element_type* map(bool writeOnly = false)
			{
				return static_cast<element_type*>(glMapNamedBuffer(m_glObj, writeOnly ? GL_WRITE_ONLY : GL_READ_WRITE));
			}

			const element_type* map() const
			{
				return static_cast<element_type*>(glMapNamedBuffer(m_glObj, GL_READ_ONLY));
			}

			const element_type* mapReadOnly() const
			{
				return map();
			}

			void unmap() const
			{
				glUnmapNamedBuffer(m_glObj);
			}

			void clearData()
			{
				setData(0, nullptr);
			}

		private:
			GLuint m_glObj;
			GLenum m_usage;
			size_type m_count;
		};


		template<typename VertexType>
		using VertexBuffer = Buffer<VertexType, GL_ARRAY_BUFFER>;

		template<typename IndexType>
		using IndexBuffer = Buffer<IndexType, GL_ELEMENT_ARRAY_BUFFER>;
	}
}

#endif // GRAPHICS_BUFFER_HPP