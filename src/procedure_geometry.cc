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
void find_bone_directions(std::vector<Bone*> bones){
	for (std::vector<Bone*>::iterator it = bones.begin() ; it != bones.end(); ++it){
		Bone* temp = *it;
		temp->tangentDir = normalize(temp->end->offset);
		glm::vec3 vH = temp->tangentDir;
		if(vH.x < vH.y && vH.x < vH.z){
			vH.x = 1;
		}else if(vH.y < vH.x && vH.y < vH.z){
			vH.y = 1;
		}else{
			vH.z = 1;
		}
		vH = cross(temp->tangentDir, vH);
		temp->normalDir = normalize(vH/length(vH));
		temp->binormalDir = normalize(cross(temp->tangentDir, temp->normalDir));



	}
}

void initialize_matrix (std::vector<Bone*> bones){
	std::map<int,glm::mat4> Anti_map;
	for (std::vector<Bone*>::iterator it = bones.begin() ; it != bones.end(); ++it){
		Bone* temp = *it;

			//Establish Identity Matrix
			temp->transform [0][0] = 1;
			temp->transform [1][1] = 1;
			temp->transform [2][2] = 1;
			temp->transform [3][3] = 1;
		if(temp->origin->parentID == -1){
			//Push in the Vector Transformation
			temp->transform [3][0] = temp->origin->offset.x;
			temp->transform [3][1] = temp->origin->offset.y;
			temp->transform [3][2] = temp->origin->offset.z;
			temp->rotate	= glm::mat4(glm::vec4(temp->binormalDir,0.0),
										glm::vec4(temp->normalDir,0.0),
										glm::vec4(temp->tangentDir,0.0),
										glm::vec4(0.0,0.0,0.0,1.0));
			temp->disformed = temp->rotate;
			Anti_map.insert(std::make_pair(temp->end->ID,temp->rotate));    	 	
		

		}else{
			temp->transform [3][0] = 0;
			temp->transform [3][1] = 0;
			temp->transform [3][2] = temp->length;
			if(Anti_map.find(temp->origin->parentID) != Anti_map.end()){
				temp->baseR = glm::inverse(Anti_map.at(temp->origin->parentID));
				temp->rotate	= glm::transpose(temp->baseR) * glm::mat4(glm::vec4(temp->binormalDir,0.0),
										glm::vec4(temp->normalDir,0.0),
										glm::vec4(temp->tangentDir,0.0),
										glm::vec4(0.0,0.0,0.0,1.0));
				Anti_map.insert(std::make_pair(temp->end->ID,temp->rotate));  
				temp->disformed = temp->rotate;
			}

		}



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

