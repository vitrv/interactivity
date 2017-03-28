#ifndef BONE_GEOMETRY_H
#define BONE_GEOMETRY_H

#include <ostream>
#include <vector>
#include <map>
#include <limits>
#include <glm/glm.hpp>
#include <mmdadapter.h>



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
	
	int ID;
	int parentID;	
	glm::vec3 offset;
	// std::vector<Joint*> child;
	// Value of some type, the weight probs

};
struct Bone {
	Joint origin;
	Joint end;
	std::vector<int> children;
	double length;
	glm::vec3 tangentDir;
	glm::vec3 normalDir;
	glm::vec3 binormalDir;

	glm::mat4 transform;
	glm::mat4 rotate;


};

struct Skeleton {
	Bone root; // ???
	std::vector<Joint> joints;
	std::vector<Bone> bones;
	int num_Bones;
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
	int getNumberOfBones() const 
	{ 
		return skeleton.num_Bones;
		// FIXME: return number of bones in skeleton
		// return num_Bones;
	}
	glm::vec3 getCenter() const { return 0.5f * glm::vec3(bounds.min + bounds.max); }
private:
	void computeBounds();
	void computeNormals();
};

#endif
