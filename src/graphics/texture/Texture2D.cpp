#include "Texture2D.hpp"
#include "util/type_registry.hpp"
#include "util/json_utils.hpp"
#include "content/Content.hpp"

#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "stb_image.h"

REGISTER_OBJECT_TYPE(Texture2D, "texture2D", ".rbt");

Texture2D::Texture2D() : Texture(GL_TEXTURE_2D), m_width(0), m_height(0), m_mipmaps(true) { }

void Texture2D::setData(const void* data, unsigned int w, unsigned int h, GLint imgFormat, GLenum pxFormat, GLenum pxType)
{
	bind();
	glTexImage2D(m_target, 0, imgFormat, w, h, 0, pxFormat, pxType, data);

	if (m_mipmaps) {
		glGenerateMipmap(m_target);
	}

	unbind();

	m_width = w;
	m_height = h;
}

void Texture2D::setCompressedData(const void* data, GLsizei dataSize, unsigned int w, unsigned int h, GLint format)
{
	bind();
	glCompressedTexImage2D(m_target, 0, format, w, h, 0, dataSize, data);

	if (m_mipmaps) {
		glGenerateMipmap(m_target);
	}

	unbind();

	m_width = w;
	m_height = h;
}

void Texture2D::setParams(bool mipmaps, filter filtering, wrap wrapping, float anisotropic, const glm::vec4& borderColor)
{
	m_mipmaps = mipmaps;
	GLenum minFilter, magFilter;
	switch (filtering) {
	case filter_point:
		minFilter = mipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
		magFilter = GL_NEAREST;
		break;

	case filter_bilinear:
		minFilter = mipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;
		magFilter = GL_LINEAR;
		break;

	case filter_trilinear:
		minFilter = mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
		magFilter = GL_LINEAR;
		break;
	}

	bind();
	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, wrapping);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, wrapping);
	glTexParameterfv(m_target, GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
	glTexParameterf(m_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropic);
	unbind();
}

template<>
std::unique_ptr<Texture2D> import_object<Texture2D>(const path& filename)
{
	// TODO
	boost::filesystem::ifstream file(filename, std::ios::binary);
	if (file) {
		auto newTexture = std::make_unique<Texture2D>();

		std::size_t headerSize, dataSize;

		file.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
		file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

		auto hdrStart = file.tellg();

		rbt_info header;
		file.read(reinterpret_cast<char*>(&header), sizeof(header));

		// manually seek to start of data stream (in case of mismatching header size)
		file.seekg(hdrStart + std::streamoff(headerSize));

		std::vector<unsigned char> data(dataSize);
		file.read(reinterpret_cast<char*>(data.data()), dataSize);

		newTexture->setParams(header.params.mipmaps, header.params.filter, header.params.wrap, header.params.anisotropic);

		if (header.compressed) {
			newTexture->setCompressedData(data.data(), GLsizei(data.size()), header.width, header.height, header.imgFormat);

		} else {
			int w, h, comp;
			unsigned char* imgData = stbi_load_from_memory(data.data(), int(data.size()), &w, &h, &comp, 4);

			if (imgData) {
				assert(w == header.width);
				assert(h == header.height);

				newTexture->setData(imgData, w, h, header.imgFormat, header.pxFormat, header.pxType);
			}

			stbi_image_free(imgData);
		}

		return std::move(newTexture);
	}

	return std::unique_ptr<Texture2D>();
}

void Texture2DTarget::attach(GLenum attPoint) const
{
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attPoint, GL_TEXTURE_2D, m_texture->glObj(), m_level);
}

void Texture2DTarget::detach(GLenum attPoint) const
{
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attPoint, GL_TEXTURE_2D, 0, 0);
}

void Texture2DTarget::getDimensions(unsigned int& width, unsigned int& height) const
{
	width = m_texture->width();
	height = m_texture->height();
}
