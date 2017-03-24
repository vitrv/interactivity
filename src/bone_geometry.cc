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
	Skeleton sktModel;

	sktModel.num_Bones = 0;
	int boneID, parentId;
	glm::vec3 offset;
	boneID =0;
	while(mr.getJoint(boneID, offset, parentId)){

		Joint tempJoint;
		tempJoint.ID = boneID;
		tempJoint.offset = offset;
		tempJoint.parentID = parentId;
		sktModel.joints.push_back(tempJoint);
		boneID++;
	}

	// Form the bone
	Bone bone;
	bool isParent = false;
	std::map<int,Joint> m;
	int numLoop =0;
	for (std::vector<Joint>::iterator it = sktModel.joints.begin() ; it != sktModel.joints.end(); ++it){
		Joint temp = *it;
		if(temp.parentID == -1){
			isParent = true;
		}
		if(m.find(temp.parentID) != m.end() ){
			
			//We got second joint, time to make a bone
			bone.end = *it;
			bone.origin = m.at(temp.parentID);
			sktModel.bones.push_back(bone);
			sktModel.num_Bones++;
			if(isParent){
				sktModel.root = bone;
				isParent = false;
			}
			Bone fresh;
			bone = fresh;

		}else
		{
			m.insert(std::make_pair(temp.ID, temp));
		}
		
	}
	
	// FIXME: load skeleton and blend weights from PMD file
	//        also initialize the skeleton as needed

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

