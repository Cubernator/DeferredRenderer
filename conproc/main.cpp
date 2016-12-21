#include "conproc.hpp"

#include "mesh.hpp"

int main(int argc, char* argv[])
{
	conproc proc;
	
	proc.register_processor<mesh_processor>("mesh",
		"converts every mesh found in the input file to a raw binary format. NOTE: potentially produces more than one output file per input file.",
		{ ".fbx", ".obj", ".blend" });

	return proc.process(argc, argv);
}