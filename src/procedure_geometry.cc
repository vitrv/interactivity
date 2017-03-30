#include "procedure_geometry.h"
#include "config.h"
#include <stdio.h>
#include <math.h>


int bone_index = 0;

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
void find_bone_directions(Joint* root){
	for (std::vector<Bone*>::iterator it = root->children.begin() ; it != root->children.end(); ++it){
		Bone* temp = *it;
		temp->tangentDir = temp->end->offset - temp->origin->offset;
		glm::vec3 vH = temp->tangentDir;
		if(vH.x < vH.y && vH.x < vH.z){
			vH.x = 1;
		}else if(vH.y < vH.x && vH.y < vH.z){
			vH.y = 1;
		}else{
			vH.z = 1;
		}
		vH = cross(temp->tangentDir, vH);
		temp->normalDir = vH/length(vH);
		temp->binormalDir = cross(temp->tangentDir, temp->normalDir);
	}
}

void create_skeleton(Joint* root, std::vector<glm::vec4>& skel_vertices, 
	                 std::vector<glm::uvec2>& skel_lines ){   

    for (std::vector<Bone*>::iterator it = root->children.begin();
        it != root->children.end(); ++it){

    	Bone* child = *it;

        glm::vec3 a = root->offset;
        glm::vec3 o = child->end->offset;
        glm::vec3 b = a + child->end->offset;
        
        skel_vertices.push_back(glm::vec4(a.x, a.y, a.z, 1.0));
        skel_vertices.push_back(glm::vec4(b.x, b.y, b.z, 1.0));
        skel_lines.push_back(glm::uvec2(bone_index++,bone_index++));

    	create_skeleton_t(child->end, b, skel_vertices, skel_lines);
    }	
}

void create_skeleton_t(Joint* root, glm::vec3 base_offset, 
	                 std::vector<glm::vec4>& skel_vertices, 
	                 std::vector<glm::uvec2>& skel_lines) {

        for (std::vector<Bone*>::iterator it = root->children.begin();
        it != root->children.end(); ++it){

	    	Bone* child = *it;

	        glm::vec3 a = base_offset;
	        glm::vec3 o = child->end->offset;
	        glm::vec3 b = a + child->end->offset;
	        
	        skel_vertices.push_back(glm::vec4(a.x, a.y, a.z, 1.0));
	        skel_vertices.push_back(glm::vec4(b.x, b.y, b.z, 1.0));
	        skel_lines.push_back(glm::uvec2(bone_index++,bone_index++));

	        create_skeleton_t(child->end, b, skel_vertices, skel_lines);
        }

}

