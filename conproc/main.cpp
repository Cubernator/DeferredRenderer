#include "conproc.hpp"

#include "mesh.hpp"
#include "image.hpp"

int main(int argc, char* argv[])
{
	conproc proc;
	
	proc.register_processor<mesh_processor>("mesh",
		"converts every mesh found in the input file to a raw binary format. NOTE: potentially produces more than one output file per input file.",
		{ ".fbx", ".obj", ".blend" });

	proc.register_processor<image_processor>("image",
		"converts an image to a common optimized file format (DXT-compressed or in PNG format)",
		{ ".png", ".jpg", ".jpeg", ".tga", ".bmp", ".psd", ".tif" });

	return proc.process(argc, argv);
}