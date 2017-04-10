#define GLM_SWIZZLE
#include "gui.h"
#include "config.h"
#include <jpegio.h>
#include "bone_geometry.h"
#include "procedure_geometry.h"
#include <iostream>
#include <debuggl.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp> 
#include <glm/glm.hpp>
#include <math.h>



using namespace glm;
int counter =0;

//namespace {
	// Intersect a cylinder with radius 1/2, height 1, with base centered at
	// (0, 0, 0) and up direction (0, 1, 0).
	bool GUI::IntersectCylinder(const glm::vec3& origin, const glm::vec3& direction,
			float radius, float height, float* t)
	{
		//FIXME perform proper ray-cylinder collision detection
		float ct; //Cylinder t
		float rt; //Ray t

		float a = dot(direction, direction);
		float b = dot(direction, ray_dir); 
		float c = dot(ray_dir, ray_dir); 
		float d = dot(direction, (origin - ray_origin));  
		float e = dot(ray_dir, (origin - ray_origin)); 

		ct = (dot(b,e) - dot(c,d)) / (dot(a,c) - dot(b,b));

		rt = (dot(a,e) - dot(b,d)) / (dot(a,c) - dot(b,b));

		glm::vec3 cyl_at = origin + ct * direction;
		glm::vec3 ray_at = ray_origin + rt * ray_dir; 

		float dist = length( cyl_at - ray_at); 

		if (dist > radius) return false;

		if (ct < 0 || ct > height) return false;

		return true;
	}


	bool GUI::Intersect(Joint*& root, glm::vec3& base_offset, float* t) {
        
        bool isect;

        for (std::vector<Bone*>::iterator it = root->children.begin();
             it != root->children.end(); ++it){


    		Bone* child = *it;

        	glm::vec3 a = base_offset;
        	glm::vec3 o = child->end->offset;
        	glm::vec3 b = a + child->end->offset;

        	glm::vec3 cyl_origin = a;
        	glm::vec3 cyl_dir = normalize(o);
        	isect = IntersectCylinder(cyl_origin, cyl_dir , kCylinderRadius, length(o), t);
        	if (isect) {
        		transformCylinder(child, cyl_origin, normalize(cyl_dir));
        		current_bone_ = child->end->ID;
        		return true;}

        	isect = Intersect(child->end, b, t);
            if (isect) return true;


        }

        return false;
       
   
	}

	bool GUI::Intersect_b(Joint*& root, glm::mat4 transform, float* t, bool lock) {
        
        bool isect;

        for (std::vector<Bone*>::iterator it = root->children.begin();
             it != root->children.end(); it++){


    		Bone* child = *it;

        	glm::vec4 a = transform * child->transform * 
        	              glm::vec4(0.0, 0.0, 0.0, 1.0);
        	glm::vec4 b = transform * child->transform * child->disformed
        	             * glm::vec4(0.0, 0.0, length(child->end->offset), 1.0);

        	glm::vec3 cyl_origin = glm::vec3(a.x, a.y, a.z);
        	glm::vec4 o = b - a; 
        	glm::vec3 cyl_dir = glm::vec3(o.x, o.y, o.z);

           if (child->end->ID == 107){
           //printf("Bone id: %d ,length b: %f , b.x: %f, b.y: %f, b.z: %f  \n",child->end->ID, length(child->end->offset), b.x, b.y, b.z );
           //printf("a.x: %f, a.y: %f, a.z: %f  \n", a.x, a.y, a.z );


           }

        	isect = IntersectCylinder(cyl_origin, normalize(cyl_dir) , kCylinderRadius, 
        		length(child->end->offset), t);

        	//if(child->end->ID == 0) isect = false;

        	if (lock && (child->end->ID == current_bone_)){
        		transformCylinder(child, cyl_origin, normalize(cyl_dir));
        		//printf("Bone id: %d\n", child->end->ID );
        		return true;
        	}

        	if (isect && !lock) {
        		transformCylinder(child, cyl_origin, normalize(cyl_dir));
        		//printf("Bone id: %d, length o: %f\n", child->end->ID, length(b));
        		current_bone_ = child->end->ID;
        		return true;}

        	isect = Intersect_b(child->end, 
        		    transform * child->transform * child->disformed, t, lock);
            if (isect) return true;	
        }


		//setCurrentBone(-1);
        return false;

       
   
	}
	void GUI::transformCylinder(Bone* child, const glm::vec3& origin, 
		const glm::vec3& direction ){

		cyl_draw_vertices.clear();
		norm_vertices.clear();
		binorm_vertices.clear();

		for (std::vector<glm::vec4>::iterator it = cyl_vertices.begin();
             it != cyl_vertices.end(); ++it){

			glm::vec4 vertex = *it;

		    glm::mat4 translate(0.0);
		    translate[0][0] = 1;
		    translate[1][1] = 1;
		    translate[2][2] = 1;
		    translate[3][3] = 1;
		    translate[3][0] = origin.x;
		    translate[3][1] = origin.y;
		    translate[3][2] = origin.z;

		    glm::vec3 axis = cross(glm::vec3(0.0,1.0,0.0), normalize(direction));
            float angle = glm::angle(glm::vec3(0.0,1.0,0.0), normalize(direction));


            glm::mat4 rotate = glm::rotate(angle, normalize(axis));

		    glm::mat4 scale(0.0);
		    scale[0][0] =  1;
            scale[1][1] = length(child->end->offset);
            scale[2][2] =  1;
            scale[3][3] =  1;
            
            glm::vec4 new_vert = translate * rotate * scale * vertex;

            cyl_draw_vertices.push_back(new_vert);
            norm_vertices.push_back(glm::vec4(origin, 1.0));
            norm_vertices.push_back(glm::vec4(origin + child->normalDir, 1.0));
            binorm_vertices.push_back(glm::vec4(origin, 1.0));
            binorm_vertices.push_back(glm::vec4(origin + child->binormalDir, 1.0));

		}	


	}
	void GUI::initCylinder(){

        int line_index = 0;
		cyl_vertices.push_back(glm::vec4(0.0, 0.0, kCylinderRadius, 1.0));
        cyl_vertices.push_back(glm::vec4(0.0, 1.0, kCylinderRadius, 1.0));
        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

        cyl_vertices.push_back(glm::vec4(0.0, 0.0, -kCylinderRadius, 1.0));
        cyl_vertices.push_back(glm::vec4(0.0, 1.0, -kCylinderRadius, 1.0));
        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

        cyl_vertices.push_back(glm::vec4(kCylinderRadius, 0.0, 0.0, 1.0));
        cyl_vertices.push_back(glm::vec4(kCylinderRadius, 1.0, 0.0, 1.0));
        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

        cyl_vertices.push_back(glm::vec4(-kCylinderRadius, 0.0, 0.0, 1.0));
        cyl_vertices.push_back(glm::vec4(-kCylinderRadius, 1.0, 0.0, 1.0));
        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

        float z = sqrt(pow(kCylinderRadius, 2) / 2);


        cyl_vertices.push_back(glm::vec4(z, 0.0, z, 1.0));
        cyl_vertices.push_back(glm::vec4(z, 1.0, z, 1.0));
        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

        cyl_vertices.push_back(glm::vec4(-z, 0.0, z, 1.0));
        cyl_vertices.push_back(glm::vec4(-z, 1.0, z, 1.0));
        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

        cyl_vertices.push_back(glm::vec4(z, 0.0, -z, 1.0));
        cyl_vertices.push_back(glm::vec4(z, 1.0, -z, 1.0));
        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

        cyl_vertices.push_back(glm::vec4(-z, 0.0, -z, 1.0));
        cyl_vertices.push_back(glm::vec4(-z, 1.0, -z, 1.0));
        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

        //Ring
        for(int v = 0; v <= 5; v++){
            float h = (float)v * (0.2);

	        cyl_vertices.push_back(glm::vec4(kCylinderRadius, h, 0.0, 1.0));
	        cyl_vertices.push_back(glm::vec4(z, h, z, 1.0));
	        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

	        cyl_vertices.push_back(glm::vec4(z, h, z, 1.0));
	        cyl_vertices.push_back(glm::vec4(0.0, h, kCylinderRadius, 1.0));
	        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

	        cyl_vertices.push_back(glm::vec4(0.0, h, kCylinderRadius, 1.0));
	        cyl_vertices.push_back(glm::vec4(-z, h, z, 1.0));
	        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

	        cyl_vertices.push_back(glm::vec4(-z, h, z, 1.0));
	        cyl_vertices.push_back(glm::vec4(-kCylinderRadius, h, 0.0, 1.0));
	        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));
	        
	        cyl_vertices.push_back(glm::vec4(-kCylinderRadius, h, 0.0, 1.0));
	        cyl_vertices.push_back(glm::vec4(-z, h, -z, 1.0));
	        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

	        cyl_vertices.push_back(glm::vec4(-z, h, -z, 1.0));
	        cyl_vertices.push_back(glm::vec4(0.0, h, -kCylinderRadius, 1.0));
	        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

	        cyl_vertices.push_back(glm::vec4(0.0, h, -kCylinderRadius, 1.0));
	        cyl_vertices.push_back(glm::vec4(z, h, -z, 1.0));
	        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));

	        cyl_vertices.push_back(glm::vec4(z, h, -z, 1.0));
	        cyl_vertices.push_back(glm::vec4(kCylinderRadius, h, 0.0, 1.0));
	        cyl_lines.push_back(glm::uvec2(line_index++,line_index++));
        } 
	}
//}

GUI::GUI(GLFWwindow* window)
	:window_(window)
{
	glfwSetWindowUserPointer(window_, this);
	glfwSetKeyCallback(window_, KeyCallback);
	glfwSetCursorPosCallback(window_, MousePosCallback);
	glfwSetMouseButtonCallback(window_, MouseButtonCallback);

	glfwGetWindowSize(window_, &window_width_, &window_height_);
	float aspect_ = static_cast<float>(window_width_) / window_height_;
	projection_matrix_ = glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
}

GUI::~GUI()
{
}

void GUI::assignMesh(Mesh* mesh)
{
	mesh_ = mesh;
	center_ = mesh_->getCenter();
}

void GUI::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window_, GL_TRUE);
		return ;
	}
	if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
		//FIXME save out a screenshot using SaveJPEG
	}

	if (captureWASDUPDOWN(key, action))
		return ;
	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
		float roll_speed = 1;
		if (key == GLFW_KEY_RIGHT)
			roll_speed = -roll_speed;
		else
			roll_speed = roll_speed;
		// FIXME: actually roll the bone here
		if(current_bone_ != -1){
			Bone* temp = mesh_->skeleton.bone_map.at(current_bone_);
			keyDisform(temp,roll_speed);
            float garbage;
			Intersect_b(mesh_->skeleton.root, glm::mat4(1.0), &garbage, true);;
		}
	} else if (key == GLFW_KEY_C && action != GLFW_RELEASE) {
		fps_mode_ = !fps_mode_;
	} else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_RELEASE) {
		current_bone_--;
		current_bone_ += mesh_->getNumberOfBones();
		current_bone_ %= mesh_->getNumberOfBones();
	} else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_RELEASE) {
		current_bone_++;
		current_bone_ += mesh_->getNumberOfBones();
		current_bone_ %= mesh_->getNumberOfBones();
	} else if (key == GLFW_KEY_T && action != GLFW_RELEASE) {
		transparent_ = !transparent_;
	} else if (key == GLFW_KEY_J && action != GLFW_RELEASE) {
		GUI::printToPNG();
	} 
}

void GUI::mousePosCallback(double mouse_x, double mouse_y)
{
	last_x_ = current_x_;
	last_y_ = current_y_;
	current_x_ = mouse_x;
	current_y_ = window_height_ - mouse_y;
	float delta_x = current_x_ - last_x_;
	float delta_y = current_y_ - last_y_;
	if (sqrt(delta_x * delta_x + delta_y * delta_y) < 1e-15)
		return;
	glm::vec3 mouse_direction = glm::normalize(glm::vec3(delta_x, delta_y, 0.0f));
	glm::vec2 mouse_start = glm::vec2(last_x_, last_y_);
	glm::vec2 mouse_end = glm::vec2(current_x_, current_y_);
	glm::uvec4 viewport = glm::uvec4(0, 0, window_width_, window_height_);

	bool drag_camera = drag_state_ && current_button_ == GLFW_MOUSE_BUTTON_RIGHT;
	bool drag_bone = drag_state_ && current_button_ == GLFW_MOUSE_BUTTON_LEFT;

	if (drag_camera) {
		glm::vec3 axis = glm::normalize(
				orientation_ *
				glm::vec3(mouse_direction.y, -mouse_direction.x, 0.0f)
				);
		orientation_ =
			glm::mat3(glm::rotate(rotation_speed_, axis) * glm::mat4(orientation_));
		tangent_ = glm::column(orientation_, 0);
		up_ = glm::column(orientation_, 1);
		look_ = glm::column(orientation_, 2);
	} else if (drag_bone && current_bone_ != -1) {
		// FIXME: Handle bone rotation
		Bone* temp = mesh_->skeleton.bone_map.at(current_bone_);
		dragDisform(temp,mouse_x,mouse_y);
		//return ;
	}

	// FIXME: highlight bones that have been moused over


	// Get Ray that the mouse is pointing for bone picking
    
    ray_origin = eye_;

	float x = (2.0f * mouse_x)/window_width_ - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / window_height_;
	float z = 1.0f;
	vec3 ray_nds = vec3(x,y,z);
	vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	vec4 ray_eye = inverse(projection_matrix_) * ray_clip;
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	vec3 ray_world = (inverse(view_matrix_)*ray_eye).xyz();
	ray_world = normalize(ray_world);
	//ray_world.x *= -1; 
	//ray_world.y *= -1; 
	//ray_world.z *= -1; 

	ray_dir = normalize(glm::vec3(window_width_* ray_world.x, 
	                window_height_ * ray_world.y, 1.0));

	ray_dir = ray_world;


    float t;
    glm::vec3 cyl_origin = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 cyl_dir = glm::vec3(0.0, 1.0, 0.0);

    
	//bool isect = Intersect(mesh_->skeleton.root, mesh_->skeleton.root->offset, &t);
    bool isect = Intersect_b(mesh_->skeleton.root, glm::mat4(1.0), &t, drag_bone);
    if (!isect && !drag_bone) current_bone_ = -1;

    //if (drag_bone) printf("Drag state true, current_bone %d\n", current_bone_ );
    //else printf("Drag state false, current_bone %d\n", current_bone_ );

}

void GUI::mouseButtonCallback(int button, int action, int mods)
{
	drag_state_ = (action == GLFW_PRESS);
	current_button_ = button;
}

void GUI::updateMatrices()
{
	// Compute our view, and projection matrices.
	if (fps_mode_)
		center_ = eye_ + camera_distance_ * look_;
	else
		eye_ = center_ - camera_distance_ * look_;

	view_matrix_ = glm::lookAt(eye_, center_, up_);
	light_position_ = glm::vec4(eye_, 1.0f);

	aspect_ = static_cast<float>(window_width_) / window_height_;
	projection_matrix_ =
		glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
	model_matrix_ = glm::mat4(1.0f);
}

MatrixPointers GUI::getMatrixPointers() const
{
	MatrixPointers ret;
	ret.projection = &projection_matrix_[0][0];
	ret.model= &model_matrix_[0][0];
	ret.view = &view_matrix_[0][0];
	return ret;
}

bool GUI::setCurrentBone(int i)
{
	if (i < 0 || i >= mesh_->getNumberOfBones())
		return false;
	current_bone_ = i;
	return true;
}

bool GUI::captureWASDUPDOWN(int key, int action)
{
	if (key == GLFW_KEY_W) {
		if (fps_mode_)
			eye_ += zoom_speed_ * look_;
		else
			camera_distance_ -= zoom_speed_;
		return true;
	} else if (key == GLFW_KEY_S) {
		if (fps_mode_)
			eye_ -= zoom_speed_ * look_;
		else
			camera_distance_ += zoom_speed_;
		return true;
	} else if (key == GLFW_KEY_A) {
		if (fps_mode_)
			eye_ -= pan_speed_ * tangent_;
		else
			center_ -= pan_speed_ * tangent_;
		return true;
	} else if (key == GLFW_KEY_D) {
		if (fps_mode_)
			eye_ += pan_speed_ * tangent_;
		else
			center_ += pan_speed_ * tangent_;
		return true;
	} else if (key == GLFW_KEY_DOWN) {
		if (fps_mode_)
			eye_ -= pan_speed_ * up_;
		else
			center_ -= pan_speed_ * up_;
		return true;
	} else if (key == GLFW_KEY_UP) {
		if (fps_mode_)
			eye_ += pan_speed_ * up_;
		else
			center_ += pan_speed_ * up_;
		return true;
	}
	return false;
}


// Delegrate to the actual GUI object.
void GUI::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->keyCallback(key, scancode, action, mods);
}

void GUI::MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mousePosCallback(mouse_x, mouse_y);
}

void GUI::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mouseButtonCallback(button, action, mods);
}

void GUI::printToPNG(){
	typedef unsigned char uchar;
    // we will store the image data here
    uchar *pixels;
    // the thingy we use to write files
    FILE * shot;


    // generate an array large enough to hold the pixel data 
    // (width*height*bytesPerPixel)
    pixels = new unsigned char[window_width_*window_height_*3];
    // read in the pixel data, TGA's pixels are BGR aligned
    glReadPixels(0, 0, window_width_,window_height_, 0x80E0, 
    GL_UNSIGNED_BYTE, pixels);

    // open the file for writing. If unsucessful, return 1
    std::string filename = "screenshot.tga";

    shot=fopen(filename.c_str(), "wb");


    // this is the tga header it must be in the beginning of 
    // every (uncompressed) .tga
    uchar TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
    // the header that is used to get the dimensions of the .tga
    // header[1]*256+header[0] - width
    // header[3]*256+header[2] - height
    // header[4] - bits per pixel
    // header[5] - ?
    uchar header[6]={((int)(window_width_%256)),
    ((int)(window_width_/256)),
    ((int)(window_height_%256)),
    ((int)(window_height_/256)),24,0};

    // write out the TGA header
    fwrite(TGAheader, sizeof(uchar), 12, shot);
    // write out the header
    fwrite(header, sizeof(uchar), 6, shot);
    // write the pixels
    fwrite(pixels, sizeof(uchar), 
    window_width_*window_height_*3, shot);

    // close the file
    fclose(shot);
    // free the memory
    delete [] pixels;

}
