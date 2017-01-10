#include "pixel_format_helper.hpp"

bool pixel_format_helper::find(const std::string& name, pixel_traits& format)
{
	return s_formats.get(name, format);
}

// Don't judge me... at least they're being stored in a hashtable!

keyword_helper<pixel_traits> pixel_format_helper::s_formats({
	{ "r8", get_pixel_traits<pixel::r8>() },
	{ "r16", get_pixel_traits<pixel::r16>() },
	{ "r32", get_pixel_traits<pixel::r32>() },

	{ "r8s", get_pixel_traits<pixel::r8s>() },
	{ "r16s", get_pixel_traits<pixel::r16s>() },
	{ "r32s", get_pixel_traits<pixel::r32s>() },

	{ "r32f", get_pixel_traits<pixel::r32f>() },

	{ "r8ui", get_pixel_traits<pixel::r8ui>() },
	{ "r16ui", get_pixel_traits<pixel::r16ui>() },
	{ "r32ui", get_pixel_traits<pixel::r32ui>() },

	{ "r8i", get_pixel_traits<pixel::r8i>() },
	{ "r16i", get_pixel_traits<pixel::r16i>() },
	{ "r32i", get_pixel_traits<pixel::r32i>() },

	{ "red", get_pixel_traits<pixel::red>() },

	{ "rg8", get_pixel_traits<pixel::rg8>() },
	{ "rg16", get_pixel_traits<pixel::rg16>() },
	{ "rg32", get_pixel_traits<pixel::rg32>() },

	{ "rg8s", get_pixel_traits<pixel::rg8s>() },
	{ "rg16s", get_pixel_traits<pixel::rg16s>() },
	{ "rg32s", get_pixel_traits<pixel::rg32s>() },

	{ "rg32f", get_pixel_traits<pixel::rg32f>() },

	{ "rg8ui", get_pixel_traits<pixel::rg8ui>() },
	{ "rg16ui", get_pixel_traits<pixel::rg16ui>() },
	{ "rg32ui", get_pixel_traits<pixel::rg32ui>() },

	{ "rg8i", get_pixel_traits<pixel::rg8i>() },
	{ "rg16i", get_pixel_traits<pixel::rg16i>() },
	{ "rg32i", get_pixel_traits<pixel::rg32i>() },

	{ "rg", get_pixel_traits<pixel::rg>() },

	{ "rgb8", get_pixel_traits<pixel::rgb8>() },
	{ "rgb16", get_pixel_traits<pixel::rgb16>() },
	{ "rgb32", get_pixel_traits<pixel::rgb32>() },

	{ "rgb8s", get_pixel_traits<pixel::rgb8s>() },
	{ "rgb16s", get_pixel_traits<pixel::rgb16s>() },
	{ "rgb32s", get_pixel_traits<pixel::rgb32s>() },

	{ "rgb32f", get_pixel_traits<pixel::rgb32f>() },

	{ "rgb8ui", get_pixel_traits<pixel::rgb8ui>() },
	{ "rgb16ui", get_pixel_traits<pixel::rgb16ui>() },
	{ "rgb32ui", get_pixel_traits<pixel::rgb32ui>() },

	{ "rgb8i", get_pixel_traits<pixel::rgb8i>() },
	{ "rgb16i", get_pixel_traits<pixel::rgb16i>() },
	{ "rgb32i", get_pixel_traits<pixel::rgb32i>() },

	{ "rgb", get_pixel_traits<pixel::rgb>() },
	{ "srgb", get_pixel_traits<pixel::srgb>() },
	{ "srgb8", get_pixel_traits<pixel::srgb8>() },

	{ "bgr8", get_pixel_traits<pixel::bgr8>() },
	{ "bgr16", get_pixel_traits<pixel::bgr16>() },
	{ "bgr32", get_pixel_traits<pixel::bgr32>() },

	{ "bgr8s", get_pixel_traits<pixel::bgr8s>() },
	{ "bgr16s", get_pixel_traits<pixel::bgr16s>() },
	{ "bgr32s", get_pixel_traits<pixel::bgr32s>() },

	{ "bgr32f", get_pixel_traits<pixel::bgr32f>() },

	{ "bgr8ui", get_pixel_traits<pixel::bgr8ui>() },
	{ "bgr16ui", get_pixel_traits<pixel::bgr16ui>() },
	{ "bgr32ui", get_pixel_traits<pixel::bgr32ui>() },

	{ "bgr8i", get_pixel_traits<pixel::bgr8i>() },
	{ "bgr16i", get_pixel_traits<pixel::bgr16i>() },
	{ "bgr32i", get_pixel_traits<pixel::bgr32i>() },

	{ "bgr", get_pixel_traits<pixel::bgr>() },

	{ "rgba8", get_pixel_traits<pixel::rgba8>() },
	{ "rgba16", get_pixel_traits<pixel::rgba16>() },
	{ "rgba32", get_pixel_traits<pixel::rgba32>() },

	{ "rgba8s", get_pixel_traits<pixel::rgba8s>() },
	{ "rgba16s", get_pixel_traits<pixel::rgba16s>() },
	{ "rgba32s", get_pixel_traits<pixel::rgba32s>() },

	{ "rgba32f", get_pixel_traits<pixel::rgba32f>() },

	{ "rgba8ui", get_pixel_traits<pixel::rgba8ui>() },
	{ "rgba16ui", get_pixel_traits<pixel::rgba16ui>() },
	{ "rgba32ui", get_pixel_traits<pixel::rgba32ui>() },

	{ "rgba8i", get_pixel_traits<pixel::rgba8i>() },
	{ "rgba16i", get_pixel_traits<pixel::rgba16i>() },
	{ "rgba32i", get_pixel_traits<pixel::rgba32i>() },

	{ "rgba", get_pixel_traits<pixel::rgba>() },
	{ "srgba", get_pixel_traits<pixel::srgba>() },
	{ "srgba8", get_pixel_traits<pixel::srgba8>() },

	{ "bgra8", get_pixel_traits<pixel::bgra8>() },
	{ "bgra16", get_pixel_traits<pixel::bgra16>() },
	{ "bgra32", get_pixel_traits<pixel::bgra32>() },

	{ "bgra8s", get_pixel_traits<pixel::bgra8s>() },
	{ "bgra16s", get_pixel_traits<pixel::bgra16s>() },
	{ "bgra32s", get_pixel_traits<pixel::bgra32s>() },

	{ "bgra32f", get_pixel_traits<pixel::bgra32f>() },

	{ "bgra8ui", get_pixel_traits<pixel::bgra8ui>() },
	{ "bgra16ui", get_pixel_traits<pixel::bgra16ui>() },
	{ "bgra32ui", get_pixel_traits<pixel::bgra32ui>() },

	{ "bgra8i", get_pixel_traits<pixel::bgra8i>() },
	{ "bgra16i", get_pixel_traits<pixel::bgra16i>() },
	{ "bgra32i", get_pixel_traits<pixel::bgra32i>() },

	{ "bgra", get_pixel_traits<pixel::bgra>() },

	{ "depth16", get_pixel_traits<pixel::depth16>() },
	{ "depth24", get_pixel_traits<pixel::depth24>() },
	{ "depth32", get_pixel_traits<pixel::depth32>() },

	{ "depth32f", get_pixel_traits<pixel::depth32f>() },

	{ "depthstencil8", get_pixel_traits<pixel::depthstencil8>() },
	{ "depthstencil16", get_pixel_traits<pixel::depthstencil16>() },
	{ "depthstencil32", get_pixel_traits<pixel::depthstencil32>() },

	{ "depthstencil32f", get_pixel_traits<pixel::depthstencil32f>() },

	{ "rg3b2", get_pixel_traits<pixel::rg3b2>() },
	{ "b2gr3", get_pixel_traits<pixel::b2gr3>() },
	{ "r5g6b5", get_pixel_traits<pixel::r5g6b5>() },
	{ "b5g6r5", get_pixel_traits<pixel::b5g6r5>() },
	{ "rgba4", get_pixel_traits<pixel::rgba4>() },
	{ "abgr4", get_pixel_traits<pixel::abgr4>() },
	{ "rgb5a1", get_pixel_traits<pixel::rgb5a1>() },
	{ "a1bgr5", get_pixel_traits<pixel::a1bgr5>() },
	{ "rgb10a2", get_pixel_traits<pixel::rgb10a2>() },
	{ "a2bgr10", get_pixel_traits<pixel::a2bgr10>() },
	{ "d24s8", get_pixel_traits<pixel::d24s8>() }
});
