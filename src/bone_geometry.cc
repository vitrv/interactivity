#include "config.h"
#include "bone_geometry.h"
#include "procedure_geometry.h"
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
	    	skeleton.bone_map.insert(std::make_pair(joint->ID, bone));
	    	skeleton.bones.push_back(bone);
	    	parent->children.push_back(bone);
	    	skeleton.total_bones++;

	    }

    }

    //check_skel(skeleton.root);


	find_bone_directions(skeleton.bones);
	// initialize_matrix(skeleton.bones);
	initialize_matrix1(skeleton.root, glm::mat4(1.0));

    mr.getJointWeights(skeleton.tup);
    setWeights(skeleton.tup, skeleton.weightmap, skeleton.bones);


    //test w/ jid: 1 vid: 6736 w: 0.750000
    printf("Weight: %f\n",getBoneWeight(6736, 1));


}

void Mesh::check_skel(Joint* root){
	check_skel_t(root, 0);
}

void Mesh::check_skel_t(Joint* root, int depth) {
	for (std::vector<Bone*>::iterator it = root->children.begin();
        it != root->children.end(); ++it){
		
		Bone* child = *it;


        printf("Bone: %d, Parent: %d, Siblings: %d\n", child->end->ID, root->ID, root->children.size());

	}
	for (std::vector<Bone*>::iterator it = root->children.begin();
        it != root->children.end(); ++it){
		
		Bone* child = *it;

        check_skel_t(child->end, depth + 1);
	}
}


void Mesh::updateAnimation()
{
	animated_vertices = vertices;
	//Use ordering of vertices as vertex id
	//update animated vertex w/ calculation function
	//Iterate thru joints and sum
}

float Mesh::getBoneWeight(int vid, int jid){

	if(skeleton.weightmap.find(vid) != skeleton.weightmap.end()){
    	if(skeleton.weightmap[vid].find(jid) != skeleton.weightmap[vid].end()){
    	    return skeleton.weightmap[vid][jid];
    	}
    	else return -1.0;
    }else return -1.0;
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

