#ifndef PROCEDURE_GEOMETRY_H
#define PROCEDURE_GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>
#include "bone_geometry.h"

class LineMesh;

void create_floor(std::vector<glm::vec4>& floor_vertices, std::vector<glm::uvec3>& floor_faces);
// FIXME: Add functions to generate the bone mesh.
void find_bone_directions(Skeleton bob);

void create_skeleton(Joint* root, std::vector<glm::vec4>& skel_vertices, 
					 std::vector<glm::uvec2>& skel_lines);

void create_skeleton_t(Joint* root, glm::vec3 base_offset, 
	                 std::vector<glm::vec4>& skel_vertices, 
	                 std::vector<glm::uvec2>& skel_lines);

#endif
