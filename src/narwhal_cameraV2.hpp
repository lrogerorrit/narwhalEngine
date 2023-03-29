//Credit to Javi Agenjo for the original code
#pragma once

#include "narwhal_matrix_4.hpp"
#include <glm/glm.hpp>


namespace narwhal{
	class NarwhalCameraV2
	{
	public:
		static NarwhalCameraV2* current;
		NarwhalCameraV2();
		//vectors to define the orientation of the camera
		glm::vec3 eye; //where is the camera
		glm::vec3 center; //where is it pointing
		glm::vec3 up; //the up pointing up

		//properties of the projection of the camera
		float fov;			//view angle in degrees (1/zoom)
		float aspect;		//aspect ratio (width/height)
		float near_plane;	//near plane
		float far_plane;	//far plane

		//planes
		float frustum[6][4];

		//matrices
		Matrix44 view_matrix;
		Matrix44 projection_matrix;
		Matrix44 viewprojection_matrix;

		//set as current
		void enable();
		void renderInMenu();

		//translate and rotate the camera
		void move(glm::vec3 delta);
		void moveGlobal(glm::vec3 delta);
		void rotate(float angle, const glm::vec3& axis);
		void changeDistance(float dt);


		//transform a local camera vector to world coordinates
		glm::vec3 getLocalVector(const glm::vec3& v);

		//set the info
		void setPerspective(float fov, float aspect, float near_plane, float far_plane);
		void lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
		void lookAt( const glm::vec3& eye, const glm::vec3& center);
		void lookAt(const Matrix44& m);

		//used to extract frustum planes
		void extractFrustum();

		//compute the matrices
		void updateViewMatrix();
		void updateProjectionMatrix();

		//to work between world and screen coordinates
		glm::vec3 project(glm::vec3 pos3d, float window_width, float window_height); //to project 3D points to screen coordinates
		glm::vec3 unproject(glm::vec3 coord2d, float window_width, float window_height); //to project screen coordinates to world coordinates
		float getProjectedScale(glm::vec3 pos3D, float radius); //used to know how big one unit will look at this distance
		glm::vec3 getRayDirection(int mouse_x, int mouse_y, float window_width, float window_height);

		//culling
		bool testPointInFrustum(glm::vec3 v);
		
		

		


		
	};
}

