#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "GL/glew.h"
#include "glm.hpp"

#include <utility>
#include <memory>
#include <vector>

class RenderTarget;

class FrameBuffer
{
public:
	using target = std::unique_ptr<RenderTarget>;
	using target_array = std::vector<target>;

	enum copy_mode
	{
		copy_color = GL_COLOR_BUFFER_BIT,
		copy_depth = GL_DEPTH_BUFFER_BIT,
		copy_stencil = GL_STENCIL_BUFFER_BIT,
		copy_color_depth = copy_color | copy_depth
	};

	FrameBuffer();
	~FrameBuffer();

	void setTargetsDepthStencil(target depthTarget, target stencilTarget, target_array&& colorTargets)
	{
		internalSetTargets(std::move(depthTarget), std::move(stencilTarget), nullptr, std::move(colorTargets));
	}
	void setTargetsDepthStencil(target depthStencilTarget, target_array&& colorTargets)
	{
		internalSetTargets(nullptr, nullptr, std::move(depthStencilTarget), std::move(colorTargets));
	}
	void setTargetsDepth(target depthTarget, target_array&& colorTargets)
	{
		setTargetsDepthStencil(std::move(depthTarget), nullptr, std::move(colorTargets));
	}
	void setTargetsStencil(target stencilTarget, target_array&& colorTargets)
	{
		setTargetsDepthStencil(nullptr, std::move(stencilTarget), std::move(colorTargets));
	}
	void setTargets(target_array&& colorTargets)
	{
		setTargetsDepthStencil(nullptr, nullptr, std::move(colorTargets));
	}

	void setTargetDepthStencil(target depthTarget, target stencilTarget, target colorTarget)
	{
		target_array tmp;
		tmp.push_back(std::move(colorTarget));
		internalSetTargets(std::move(depthTarget), std::move(stencilTarget), nullptr, std::move(tmp));
	}
	void setTargetDepthStencil(target depthStencilTarget, target colorTarget)
	{
		target_array tmp;
		tmp.push_back(std::move(colorTarget));
		internalSetTargets(nullptr, nullptr, std::move(depthStencilTarget), std::move(tmp));
	}
	void setTargetDepth(target depthTarget, target colorTarget)
	{
		target_array tmp;
		tmp.push_back(std::move(colorTarget));
		setTargetsDepthStencil(std::move(depthTarget), nullptr, std::move(tmp));
	}
	void setTargetStencil(target stencilTarget, target colorTarget)
	{
		target_array tmp;
		tmp.push_back(std::move(colorTarget));
		setTargetsDepthStencil(nullptr, std::move(stencilTarget), std::move(tmp));
	}
	void setTarget(target colorTarget)
	{
		target_array tmp;
		tmp.push_back(std::move(colorTarget));
		setTargetsDepthStencil(nullptr, nullptr, std::move(tmp));
	}

	void setClearColor(unsigned int i, glm::vec4 color) { m_clearColors[i] = color; }
	void setClearDepth(float v) { m_clearDepth = v; }
	void setClearStencil(int v) { m_clearStencil = v; }

	GLuint glObj() const { return m_glObj; }

	unsigned int colorTargetCount() const { return m_colorTargets.size(); }

	void bind() const;
	static void unbind();

	void clear() const;

private:
	GLuint m_glObj;

	target m_depthTarget, m_stencilTarget, m_depthStencilTarget;
	target_array m_colorTargets;

	float m_clearDepth;
	int m_clearStencil;
	std::vector<glm::vec4> m_clearColors;

	void bind(GLenum t) const;
	static void unbind(GLenum t);

	void attachTargets();
	void detachTargets();
	void internalSetTargets(target d, target s, target ds, target_array&& c);

	void setDrawBuffers(const std::vector<GLenum>& bufs);
	void setDrawAll();

	void setReadBuffer(GLenum buf);
};

#endif // FRAMEBUFFER_HPP