#ifndef RBT_HPP
#define RBT_HPP

#include "GL/glew.h"

enum rbt_filter
{
	filter_point,
	filter_bilinear,
	filter_trilinear
};

enum rbt_wrap
{
	wrap_repeat = GL_REPEAT,
	wrap_clampToEdge = GL_CLAMP_TO_EDGE,
	wrap_clampToBorder = GL_CLAMP_TO_BORDER,
	wrap_mirroredRepeat = GL_MIRRORED_REPEAT,
	wrap_mirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
};

struct rbt_params
{
	bool mipmaps;
	rbt_filter filter;
	rbt_wrap wrap;
	float anisotropic;
};

struct rbt_info
{
	bool compressed;
	GLint imgFormat;
	GLenum pxFormat;
	GLenum pxType;
	unsigned int width;
	unsigned int height;
	rbt_params params;
};

#endif // RBT_HPP