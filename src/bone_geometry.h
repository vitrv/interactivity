#ifndef BONE_GEOMETRY_H
#define BONE_GEOMETRY_H

#include <ostream>
#include <vector>
#include <map>
#include <limits>
#include <glm/glm.hpp>
#include <mmdadapter.h>


struct Bone;

struct BoundingBox {
	BoundingBox()
		: min(glm::vec3(-std::numeric_limits<float>::max())),
		max(glm::vec3(std::numeric_limits<float>::max())) {}
	glm::vec3 min;
	glm::vec3 max;
};



struct Joint {
	// FIXME: Implement your Joint data structure.
	// Note: PMD represents weights on joints, but you need weights on
	//       bones to calculate the actual animation.
    Joint(int _ID, glm::vec3 _offset, int _parentID) : ID(_ID), 
          offset(_offset), parentID(_parentID) {}

	Bone* parentBone;
	int ID;
	int parentID;	
	glm::vec3 offset;
	std::vector<Bone*> children;

};
struct Bone {
    Bone(Joint* _end, Joint* _origin) :
        end(_end), origin(_origin) {}

	Joint* origin;
	Joint* end;
	std::vector<SparseTuple*> tup;

	double length;

	glm::vec3 tangentDir;
	glm::vec3 normalDir;
	glm::vec3 binormalDir;
	glm::mat4 transform;
	glm::mat4 disformed;
	glm::mat4 rotate;
	// Parent Bone's Inverse Rotation Matrix;
	glm::mat4 baseR;


};

struct Skeleton {
	Joint* root;
	std::vector<Joint*> joints;
	std::vector<Bone*> bones;
	std::map<int,Bone*> bone_map;
	int total_bones;

	~Skeleton(){
		for (std::vector<Joint*>::iterator it = joints.begin();
         it != joints.end(); ++it){

         	delete *it;
        } 	

        for (std::vector<Bone*>::iterator it = bones.begin();
         it != bones.end(); ++it){
         	delete *it;
        } 	
	}
	// FIXME: create skeleton and bone data structures
};





struct Mesh {
	Mesh();
	~Mesh();
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> animated_vertices;
	std::vector<glm::uvec3> faces;
	std::vector<glm::vec4> vertex_normals;
	std::vector<glm::vec4> face_normals;
	std::vector<glm::vec2> uv_coordinates;
	std::vector<Material> materials;
	BoundingBox bounds;
	Skeleton skeleton;

	void loadpmd(const std::string& fn);
	void updateAnimation();
	void check_skel(Joint* root);
	void check_skel_t(Joint* root, int depth);
	int getNumberOfBones() const 
	{ 
		return skeleton.total_bones;
		// FIXME: return number of bones in skeleton
		// return num_Bones;
	}
	glm::vec3 getCenter() const { return 0.5f * glm::vec3(bounds.min + bounds.max); }

private:
	void computeBounds();
	void computeNormals();
};

#endif
