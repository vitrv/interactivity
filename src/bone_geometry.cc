#include "config.h"
#include "bone_geometry.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <glm/gtx/io.hpp>
#include <glm/gtx/transform.hpp>

/*
 * For debugging purpose.
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	size_t count = std::min(v.size(), static_cast<size_t>(10));
	for (size_t i = 0; i < count; ++i) os << i << " " << v[i] << "\n";
	os << "size = " << v.size() << "\n";
	return os;
}

std::ostream& operator<<(std::ostream& os, const BoundingBox& bounds)
{
	os << "min = " << bounds.min << " max = " << bounds.max;
	return os;
}



// FIXME: Implement bone animation.


Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::loadpmd(const std::string& fn)
{
	MMDReader mr;
	mr.open(fn);
	mr.getMesh(vertices, faces, vertex_normals, uv_coordinates);
	computeBounds();
	mr.getMaterial(materials);
	
	//Skeleton skel_model;
	//skel_model.total_bones = 0;
	int jointID, parentID;
	glm::vec3 offset;
	jointID = 0;

	std::map<int,Joint*> joint_map;

	std::vector<Joint*> joint_list;

    while(mr.getJoint(jointID, offset, parentID)){

        Joint* joint = new Joint(jointID, offset, parentID);
        skeleton.joints.push_back(joint);
        joint_list.push_back(joint);
        joint_map.insert(std::make_pair(jointID, joint));
        
        jointID+= 1;
    }

    skeleton.total_bones = 0;
    for (std::vector<Joint*>::iterator it = joint_list.begin();
         it != joint_list.end(); ++it){
		
		Joint* joint = *it;

 
	    if(joint->parentID == -1){
			skeleton.root = joint;
		}
	    else if (joint_map.find(joint->parentID) != joint_map.end()){

            Joint* parent = joint_map.at(joint->parentID);

	    	Bone* bone = new Bone(joint, parent);

	    	skeleton.bones.push_back(bone);
	    	parent->children.push_back(bone);
	    	skeleton.total_bones++;

	    }

    }

}

void Mesh::updateAnimation()
{
	animated_vertices = vertices;
	// FIXME: blend the vertices to animated_vertices, rather than copy
	//        the data directly.
}


void Mesh::computeBounds()
{
	bounds.min = glm::vec3(std::numeric_limits<float>::max());
	bounds.max = glm::vec3(-std::numeric_limits<float>::max());
	for (const auto& vert : vertices) {
		bounds.min = glm::min(glm::vec3(vert), bounds.min);
		bounds.max = glm::max(glm::vec3(vert), bounds.max);
	}
}

