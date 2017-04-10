#include "procedure_geometry.h"
#include "config.h"
#include <stdio.h>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

bool firstRender = true;
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
		if(abs(vH.x) < abs(vH.y) && abs(vH.x) < abs(vH.z)){
			vH.x = 1;
			vH.y = 0;
			vH.z = 0;
		}else if(abs(vH.y) < abs(vH.x) && abs(vH.y) < abs(vH.z)){
			vH.y = 1;
			vH.x = 0;
			vH.z = 0;
		}else{
			vH.z = 1;
			vH.y = 0;
			vH.x = 0;
		}
		vH = cross(temp->tangentDir, vH);
		if (length(vH) < .00001) vH = glm::vec3(1.0, 0.0, 0.0);
		temp->normalDir = normalize(vH/length(vH));
		temp->binormalDir = normalize(cross(temp->tangentDir, temp->normalDir));

		//printf("bone: %d , normal: %f, binormal: %f , tangent: %f\n", temp->end->ID, length(temp->normalDir), length(temp->binormalDir), length(temp->tangentDir) );

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
			temp->transform [3][2] = length(temp->origin->offset);
			if(Anti_map.find(temp->origin->parentID) != Anti_map.end()){
				temp->baseR = (Anti_map.at(temp->origin->parentID));
				temp->rotate	= glm::transpose(temp->baseR) * glm::mat4(glm::vec4(temp->binormalDir,0.0),
										glm::vec4(temp->normalDir,0.0),
										glm::vec4(temp->tangentDir,0.0),
										glm::vec4(0.0,0.0,0.0,1.0));
				temp->baseR *= temp->rotate;
				Anti_map.insert(std::make_pair(temp->end->ID,temp->baseR));  
				temp->disformed = temp->rotate;
			}

		}



	}
}

void initialize_matrix1 (Joint* root, glm::mat4 baseR){


	for (std::vector<Bone*>::iterator it = root->children.begin();
        it != root->children.end(); ++it){
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

		}else{
			temp->transform [3][0] = 0;
			temp->transform [3][1] = 0;
			temp->transform [3][2] = length(temp->origin->offset);
			
				temp->rotate	= glm::transpose(baseR) * glm::mat4(glm::vec4(temp->binormalDir,0.0),
										glm::vec4(temp->normalDir,0.0),
										glm::vec4(temp->tangentDir,0.0),
										glm::vec4(0.0,0.0,0.0,1.0));
			
				
				temp->disformed = temp->rotate;

		}

		temp->transformU = baseR * temp->transform * temp->rotate;
		initialize_matrix1(temp->end, baseR * temp->rotate);




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

void create_skeleton1(Joint* root, std::vector<glm::vec4>& skel_vertices, 
					 std::vector<glm::uvec2>& skel_lines){
	skel_vertices.clear();
	create_skeleton_t1(root, glm::mat4(1.0),skel_vertices, skel_lines);
}

void create_skeleton_t1(Joint* root, glm::mat4 transform, 
	                 std::vector<glm::vec4>& skel_vertices, 
	                 std::vector<glm::uvec2>& skel_lines) {
	for (std::vector<Bone*>::iterator it = root->children.begin();
        it != root->children.end(); ++it){
		Bone* temp = *it;

		glm::vec4 startPoint = transform * temp->transform * glm::vec4(0.0, 0.0, 0.0, 1.0);
		glm::vec4 endPoint = transform * temp->transform * temp->disformed * glm::vec4(0.0, 0.0, length(temp->end->offset), 1.0);

		temp->transformD = transform * temp->transform;


		skel_vertices.push_back(startPoint);
        skel_vertices.push_back(endPoint);
        if(firstRender){
        	skel_lines.push_back(glm::uvec2(bone_index++,bone_index++));        	
        }

        create_skeleton_t1(temp->end,(transform * temp->transform * temp->disformed), skel_vertices, skel_lines );
	}

}
void setFirstrender(){
	firstRender = false;
}

double oldX =0;
double oldY =0;

void dragDisform(Bone* temp, double x, double y){
	//Calculate left or right
	double nx = ((x-oldX) > 0) - ((x-oldX) < 0);
	double ny = ((y-oldY) > 0) - ((y-oldY) < 0);
	oldX = x;
	oldY = y;
	float xangle = 0.01f*nx;
	float yangle = 0.01f*ny;
    glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), xangle, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotatey = glm::rotate(glm::mat4(1.0f), yangle, glm::vec3(0.0f, 1.0f, 0.0f));
    //printf("nx is:%f ny is:%f\n",nx, ny );
    if(nx && ny )
		temp->disformed *= rotateX * rotatey;
	
}

void keyDisform(Bone* temp, int x){
	//Calculate left or right
	
	float xangle = 0.01f*x;
    glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), xangle, temp->binormalDir);

    glm::vec4 n = rotateX * glm::vec4(temp->normalDir, 0.0);
    glm::vec4 b = rotateX * glm::vec4(temp->binormalDir, 0.0);

    temp->normalDir = glm::vec3(n.x, n.y, n.z);
    temp->binormalDir = glm::vec3(b.x, b.y, b.z);

	temp->disformed *= rotateX;
	
}

//std::map<int jointID, std::map<int vertID, float weight>> weightmap,
void setWeights(std::vector<SparseTuple> tup,
                std::map<int, std::map<int, float>>& weightmap,
                std::vector<Bone*> bones){

	
    for (std::vector<SparseTuple>::iterator it = tup.begin() ; it != tup.end(); ++it){
		SparseTuple tuple = *it;

		//printf("jid: %d vid: %d w: %f\n", tuple.jid, tuple.vid, tuple.weight );
		std::map<int, std::map<int, float>>::iterator mapIt = weightmap.find(tuple.vid);
		if(mapIt != weightmap.end()){
			weightmap.at(tuple.vid).insert(std::make_pair(tuple.jid, tuple.weight));

		}
		else{
			
			std::map<int, float > temp;
			temp.insert(std::make_pair(tuple.jid, tuple.weight));
			weightmap.insert(std::make_pair(tuple.vid, temp));

		}
    }
}
