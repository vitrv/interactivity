#include "procedure_geometry.h"
#include "config.h"
#include <stdio.h>
#include <math.h>

void create_floor(std::vector<glm::vec4>& floor_vertices, std::vector<glm::uvec3>& floor_faces)
{
	floor_vertices.push_back(glm::vec4(kFloorXMin, kFloorY, kFloorZMax, 1.0f));
	floor_vertices.push_back(glm::vec4(kFloorXMax, kFloorY, kFloorZMax, 1.0f));
	floor_vertices.push_back(glm::vec4(kFloorXMax, kFloorY, kFloorZMin, 1.0f));
	floor_vertices.push_back(glm::vec4(kFloorXMin, kFloorY, kFloorZMin, 1.0f));
	floor_faces.push_back(glm::uvec3(0, 1, 2));
	floor_faces.push_back(glm::uvec3(2, 3, 0));
}

// FIXME: create cylinders and lines for the bones
// Hints: Generate a lattice in [-0.5, 0, 0] x [0.5, 1, 0] We wrap this
// around in the vertex shader to produce a very smooth cylinder.  We only
// need to send a small number of points.  Controlling the grid size gives a
// nice wireframe.
void find_bone_directions(Skeleton bob){
	for (std::vector<Bone>::iterator it = bob.bones.begin() ; it != bob.bones.end(); ++it){
		Bone temp = *it;
		
	}
}

void create_skeleton(Skeleton skel, std::vector<glm::vec4>& skel_vertices, 
	                 std::vector<glm::uvec2>& skel_lines ){

    glm::vec3 base_offset = skel.joints[0].offset;

    skel_vertices.push_back(glm::vec4(0.0,0.0,0.0,1.0));
	skel_vertices.push_back(glm::vec4(0.0,5.0,0.0,1.0));
	skel_lines.push_back(glm::uvec2(0,1));
}
