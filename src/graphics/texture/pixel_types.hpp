#ifndef PIXEL_TYPES_HPP
#define PIXEL_TYPES_HPP

#include <cstdint>
#include <cstdio>

#include "graphics/gl_types.hpp"

namespace pixel
{
	namespace detail
	{
		template<GLint imgFormat, GLenum pxFormat, GLenum pxType>
		struct fmt
		{
			static constexpr GLint img_format = imgFormat;
			static constexpr GLenum px_format = pxFormat;
			static constexpr GLenum px_type = pxType;
		};
	}

	// Generic type templates

	template<typename T, GLint imgFormat = GL_RED, bool integral = false>
	struct tred : public detail::fmt<imgFormat, integral ? GL_RED_INTEGER : GL_RED, gl_type<T>()>
	{
		using value_type = T;
		value_type r;

		tred(const value_type& r) : r(r) { }
	};

	template<typename T, GLint imgFormat = GL_RG, bool integral = false>
	struct trg : public detail::fmt<imgFormat, integral ? GL_RG_INTEGER : GL_RG, gl_type<T>()>
	{
		using value_type = T;
		value_type r, g;

		trg(const value_type& r, const value_type& g) : r(r), g(g) { }
	};

	template<typename T, GLint imgFormat = GL_RGB, bool integral = false>
	struct trgb : public detail::fmt<imgFormat, integral ? GL_RGB_INTEGER : GL_RGB, gl_type<T>()>
	{
		using value_type = T;
		value_type r, g, b;

		trgb(const value_type& r, const value_type& g, const value_type& b) : r(r), g(g), b(b) { }
	};

	template<typename T, GLint imgFormat = GL_RGB, bool integral = false>
	struct tbgr : public detail::fmt<imgFormat, integral ? GL_BGR_INTEGER : GL_BGR, gl_type<T>()>
	{
		using value_type = T;
		value_type b, g, r;

		tbgr(const value_type& b, const value_type& g, const value_type& r) : b(b), g(g), r(r) { }
	};

	template<typename T, GLint imgFormat = GL_RGBA, bool integral = false>
	struct trgba : public detail::fmt<imgFormat, integral ? GL_RGBA_INTEGER : GL_RGBA, gl_type<T>()>
	{
		using value_type = T;
		value_type r, g, b, a;

		trgba(const value_type& r, const value_type& g, const value_type& b, const value_type& a) : r(r), g(g), b(b), a(a) { }
	};

	template<typename T, GLint imgFormat = GL_RGBA, bool integral = false>
	struct tbgra : public detail::fmt<imgFormat, integral ? GL_BGRA_INTEGER : GL_BGRA, gl_type<T>()>
	{
		using value_type = T;
		value_type b, g, r, a;

		tbgra(const value_type& b, const value_type& g, const value_type& r, const value_type& a) : b(b), g(g), r(r), a(a) { }
	};

	template<typename T, GLint imgFormat = GL_DEPTH_COMPONENT>
	struct tdepth : public detail::fmt<imgFormat, GL_DEPTH_COMPONENT, gl_type<T>()>
	{
		using value_type = T;
		value_type d;

		tdepth(const value_type& d) : d(d) { }
	};

	template<typename T, GLint imgFormat = GL_DEPTH_STENCIL>
	struct tdepthstencil : public detail::fmt<imgFormat, GL_DEPTH_STENCIL, gl_type<T>()>
	{
		using value_type = T;
		value_type d, s;

		tdepthstencil(const value_type& d, const value_type& s) : d(d), s(s) { }
	};

	template<typename T, GLint imgFormat, GLenum pxFormat, GLenum pxType>
	struct packed : detail::fmt<imgFormat, pxFormat, pxType>
	{
		using data_type = T;
		data_type data;

		packed(const data_type& v) : data(d) { }
	};


	// Single component types

	using r8 = tred<u8_t, GL_R8>;
	using r16 = tred<u16_t, GL_R16>;
	using r32 = tred<u32_t>;

	using r8s = tred<s8_t, GL_R8_SNORM>;
	using r16s = tred<s16_t, GL_R16_SNORM>;
	using r32s = tred<s32_t>;

	using r16f = tred<f16_t, GL_R16F>;
	using r32f = tred<f32_t, GL_R32F>;

	using r8ui = tred<u8_t, GL_R8UI, true>;
	using r16ui = tred<u16_t, GL_R16UI, true>;
	using r32ui = tred<u32_t, GL_R32UI, true>;

	using r8i = tred<s8_t, GL_R8I, true>;
	using r16i = tred<s16_t, GL_R16I, true>;
	using r32i = tred<s32_t, GL_R32I, true>;

	using red = r8;


	// Two component types

	using rg8 = trg<u8_t, GL_RG8>;
	using rg16 = trg<u16_t, GL_RG16>;
	using rg32 = trg<u32_t>;

	using rg8s = trg<s8_t, GL_RG8_SNORM>;
	using rg16s = trg<s16_t, GL_RG16_SNORM>;
	using rg32s = trg<s32_t>;

	using rg16f = trg<f16_t, GL_RG16F>;
	using rg32f = trg<f32_t, GL_RG32F>;

	using rg8ui = trg<u8_t, GL_RG8UI, true>;
	using rg16ui = trg<u16_t, GL_RG16UI, true>;
	using rg32ui = trg<u32_t, GL_RG32UI, true>;

	using rg8i = trg<s8_t, GL_RG8I, true>;
	using rg16i = trg<s16_t, GL_RG16I, true>;
	using rg32i = trg<s32_t, GL_RG32I, true>;

	using rg = rg8;


	// Three component types

	using rgb8 = trgb<u8_t, GL_RGB8>;
	using rgb16 = trgb<u16_t, GL_RGB16>;
	using rgb32 = trgb<u32_t>;

	using rgb8s = trgb<s8_t, GL_RGB8_SNORM>;
	using rgb16s = trgb<s16_t, GL_RGB16_SNORM>;
	using rgb32s = trgb<s32_t>;

	using rgb16f = trgb<f16_t, GL_RGB16F>;
	using rgb32f = trgb<f32_t, GL_RGB32F>;

	using rgb8ui = trgb<u8_t, GL_RGB8UI, true>;
	using rgb16ui = trgb<u16_t, GL_RGB16UI, true>;
	using rgb32ui = trgb<u32_t, GL_RGB32UI, true>;

	using rgb8i = trgb<s8_t, GL_RGB8I, true>;
	using rgb16i = trgb<s16_t, GL_RGB16I, true>;
	using rgb32i = trgb<s32_t, GL_RGB32I, true>;

	using rgb = trgb<u8_t, GL_RGB>;
	using srgb = trgb<u8_t, GL_SRGB>;
	using srgb8 = trgb<u8_t, GL_SRGB8>;


	// Reverse three component types

	using bgr8 = tbgr<u8_t, GL_RGB8>;
	using bgr16 = tbgr<u16_t, GL_RGB16>;
	using bgr32 = tbgr<u32_t>;

	using bgr8s = tbgr<s8_t, GL_RGB8_SNORM>;
	using bgr16s = tbgr<s16_t, GL_RGB16_SNORM>;
	using bgr32s = tbgr<s32_t>;

	using bgr16f = tbgr<f16_t, GL_RGB16F>;
	using bgr32f = tbgr<f32_t, GL_RGB32F>;

	using bgr8ui = tbgr<u8_t, GL_RGB8UI, true>;
	using bgr16ui = tbgr<u16_t, GL_RGB16UI, true>;
	using bgr32ui = tbgr<u32_t, GL_RGB32UI, true>;

	using bgr8i = tbgr<s8_t, GL_RGB8I, true>;
	using bgr16i = tbgr<s16_t, GL_RGB16I, true>;
	using bgr32i = tbgr<s32_t, GL_RGB32I, true>;

	using bgr = tbgr<u8_t, GL_RGB>;

	// Four component types

	using rgba8 = trgba<u8_t, GL_RGBA8>;
	using rgba16 = trgba<u16_t, GL_RGBA16>;
	using rgba32 = trgba<u32_t>;

	using rgba8s = trgba<s8_t, GL_RGBA8_SNORM>;
	using rgba16s = trgba<s16_t, GL_RGBA16_SNORM>;
	using rgba32s = trgba<s32_t>;

	using rgba16f = trgba<f16_t, GL_RGBA16F>;
	using rgba32f = trgba<f32_t, GL_RGBA32F>;

	using rgba8ui = trgba<u8_t, GL_RGBA8UI, true>;
	using rgba16ui = trgba<u16_t, GL_RGBA16UI, true>;
	using rgba32ui = trgba<u32_t, GL_RGBA32UI, true>;

	using rgba8i = trgba<s8_t, GL_RGBA8I, true>;
	using rgba16i = trgba<s16_t, GL_RGBA16I, true>;
	using rgba32i = trgba<s32_t, GL_RGBA32I, true>;

	using rgba = trgba<u8_t, GL_RGBA>;
	using srgba = trgba<u8_t, GL_SRGB_ALPHA>;
	using srgba8 = trgba<u8_t, GL_SRGB8_ALPHA8>;


	// Reverse four component types

	using bgra8 = tbgra<u8_t, GL_RGBA8>;
	using bgra16 = tbgra<u16_t, GL_RGBA16>;
	using bgra32 = tbgra<u32_t>;

	using bgra8s = tbgra<s8_t, GL_RGBA8_SNORM>;
	using bgra16s = tbgra<s16_t, GL_RGBA16_SNORM>;
	using bgra32s = tbgra<s32_t>;

	using bgra16f = tbgra<f16_t, GL_RGBA16F>;
	using bgra32f = tbgra<f32_t, GL_RGBA32F>;

	using bgra8ui = tbgra<u8_t, GL_RGBA8UI, true>;
	using bgra16ui = tbgra<u16_t, GL_RGBA16UI, true>;
	using bgra32ui = tbgra<u32_t, GL_RGBA32UI, true>;

	using bgra8i = tbgra<s8_t, GL_RGBA8I, true>;
	using bgra16i = tbgra<s16_t, GL_RGBA16I, true>;
	using bgra32i = tbgra<s32_t, GL_RGBA32I, true>;

	using bgra = bgra8;


	// Depth types

	using depth16 = tdepth<u16_t, GL_DEPTH_COMPONENT16>;
	using depth24 = tdepth<u32_t, GL_DEPTH_COMPONENT24>;
	using depth32 = tdepth<u32_t, GL_DEPTH_COMPONENT32>;

	using depth32f = tdepth<f32_t, GL_DEPTH_COMPONENT32F>;


	// Depth + stencil types

	using depthstencil8 = tdepthstencil<u8_t>;
	using depthstencil16 = tdepthstencil<u16_t>;
	using depthstencil32 = tdepthstencil<u32_t>;

	using depthstencil32f = tdepthstencil<f32_t>;


	// Packed types

	using rg3b2		= packed<u8_t,	GL_R3_G3_B2,			GL_RGB,				GL_UNSIGNED_BYTE_3_3_2>;
	using b2gr3		= packed<u8_t,	GL_R3_G3_B2,			GL_RGB,				GL_UNSIGNED_BYTE_2_3_3_REV>;
	using r5g6b5	= packed<u16_t, GL_RGB,					GL_RGB,				GL_UNSIGNED_SHORT_5_6_5>;
	using b5g6r5	= packed<u16_t, GL_RGB,					GL_RGB,				GL_UNSIGNED_SHORT_5_6_5_REV>;
	using rgba4		= packed<u16_t, GL_RGBA4,				GL_RGBA,			GL_UNSIGNED_SHORT_4_4_4_4>;
	using abgr4		= packed<u16_t, GL_RGBA4,				GL_RGBA,			GL_UNSIGNED_SHORT_4_4_4_4_REV>;
	using rgb5a1	= packed<u16_t, GL_RGB5_A1,				GL_RGBA,			GL_UNSIGNED_SHORT_5_5_5_1>;
	using a1bgr5	= packed<u16_t, GL_RGB5_A1,				GL_RGBA,			GL_UNSIGNED_SHORT_1_5_5_5_REV>;
	using rgb10a2	= packed<u32_t, GL_RGB10_A2,			GL_RGBA,			GL_UNSIGNED_INT_10_10_10_2>;
	using a2bgr10	= packed<u32_t, GL_RGB10_A2,			GL_RGBA,			GL_UNSIGNED_INT_2_10_10_10_REV>;
	using d24s8		= packed<u32_t, GL_DEPTH24_STENCIL8,	GL_DEPTH_STENCIL,	GL_UNSIGNED_INT_24_8>;
}

struct pixel_traits
{
	GLint imageFormat;
	GLenum pixelFormat;
	GLenum pixelType;
	std::size_t pixelSize;
};

template<typename T>
pixel_traits get_pixel_traits()
{
	return pixel_traits{T::img_format, T::px_format, T::px_type, sizeof(T)};
}

template<typename T>
constexpr GLint image_format() { return T::img_format; }

template<typename T>
constexpr GLenum pixel_format() { return T::px_format; }

template<typename T>
constexpr GLenum pixel_type() { return T::px_type; }

#endif // PIXEL_TYPES_HPP