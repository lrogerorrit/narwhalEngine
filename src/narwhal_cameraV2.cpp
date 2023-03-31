//Credit to Javi Agenjo for the original code
#include "narwhal_cameraV2.hpp"



#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

//std
#include <iostream>

namespace narwhal {
	NarwhalCameraV2* NarwhalCameraV2::current = NULL;

	NarwhalCameraV2::NarwhalCameraV2()
	{
		//lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

		
	}

	void NarwhalCameraV2::enable()
	{
		current = this;
	}

	void NarwhalCameraV2::updateViewMatrix()
	{
		view_matrix.lookAt(eye, center, up);
		viewprojection_matrix = view_matrix * projection_matrix;
		extractFrustum();
	}

	// ******************************************

	//Create a projection matrix
	void NarwhalCameraV2::updateProjectionMatrix()
	{
		projection_matrix.perspective(fov, aspect, near_plane, far_plane);
		viewprojection_matrix = view_matrix * projection_matrix;

		extractFrustum();
	}

	glm::vec3 NarwhalCameraV2::getLocalVector(const glm::vec3& v)
	{
		Matrix44 iV = view_matrix;
		if (iV.inverse() == false)
			std::cout << "Matrix Inverse error" << std::endl;
		glm::vec3 result = iV.rotateVector(v);
		return result;
	}

	void NarwhalCameraV2::move(glm::vec3 delta)
	{
		glm::vec3 localDelta = getLocalVector(delta);
		eye = eye - localDelta;
		center = center - localDelta;
		updateViewMatrix();
	}

	void NarwhalCameraV2::moveGlobal(glm::vec3 delta)
	{
		eye = eye - delta;
		center = center - delta;
		updateViewMatrix();
	}

	void NarwhalCameraV2::rotate(float angle, const glm::vec3& axis)
	{
		Matrix44 R;
		R.setRotation(angle, axis);
		glm::vec3 new_front = R * (center - eye);
		center = eye + new_front;
		updateViewMatrix();

	}


	void NarwhalCameraV2::changeDistance(float dt)
	{
		
		glm::vec3 dist = eye - center;
		dist *= dt < 0 ? 1.1 : 0.9;
		eye = dist + center;

		updateViewMatrix();
	}


	void NarwhalCameraV2::setPerspective(float fov, float aspect, float near_plane, float far_plane)
	{

		this->fov = fov;
		this->aspect = aspect;
		this->near_plane = near_plane;
		this->far_plane = far_plane;

		//update projection
		updateProjectionMatrix();
	}

	void NarwhalCameraV2::lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
	{
		this->eye = eye;
		this->center = center;
		this->up = up;

		updateViewMatrix();
	}

	void NarwhalCameraV2::lookAt(const glm::vec3& eye, const glm::vec3& center) {
		this->eye= eye;
		this->center = center;
		//Get up vector using eye (the camera) and center (the point we are looking at)
		glm::vec3 direction= glm::normalize(center - eye);
		glm::vec3 right= glm::cross(direction,glm::vec3(0,-1,0));
		this->up = glm::cross(right, direction);
		this->up = this->up * glm::vec3(1, 1, -1);
		updateViewMatrix();

	}

	void NarwhalCameraV2::lookAt(const Matrix44& m)
	{
		this->eye = m * glm::vec3();
		this->center = m * glm::vec3(0, 0, -1);
		this->up = m.rotateVector(glm::vec3(0, 1, 0));
	}

	void NarwhalCameraV2::extractFrustum()
	{
		float   proj[16] = {};
		float   modl[16] = {};
		float   clip[16] = {};
		float   t;

		Matrix44 v = view_matrix;

		memcpy(proj, projection_matrix.m, sizeof(Matrix44));
		memcpy(modl, v.m, sizeof(Matrix44));

		/* Combine the two matrices (multiply projection by modelview) */
		clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
		clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
		clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
		clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

		clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
		clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
		clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
		clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

		clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
		clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
		clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
		clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

		clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
		clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
		clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
		clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

		/* Extract the numbers for the RIGHT plane */
		frustum[0][0] = clip[3] - clip[0];
		frustum[0][1] = clip[7] - clip[4];
		frustum[0][2] = clip[11] - clip[8];
		frustum[0][3] = clip[15] - clip[12];

		/* Normalize the result */
		t = sqrt(frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2]);
		frustum[0][0] /= t;
		frustum[0][1] /= t;
		frustum[0][2] /= t;
		frustum[0][3] /= t;

		/* Extract the numbers for the LEFT plane */
		frustum[1][0] = clip[3] + clip[0];
		frustum[1][1] = clip[7] + clip[4];
		frustum[1][2] = clip[11] + clip[8];
		frustum[1][3] = clip[15] + clip[12];

		/* Normalize the result */
		t = sqrt(frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2]);
		frustum[1][0] /= t;
		frustum[1][1] /= t;
		frustum[1][2] /= t;
		frustum[1][3] /= t;

		/* Extract the BOTTOM plane */
		frustum[2][0] = clip[3] + clip[1];
		frustum[2][1] = clip[7] + clip[5];
		frustum[2][2] = clip[11] + clip[9];
		frustum[2][3] = clip[15] + clip[13];

		/* Normalize the result */
		t = sqrt(frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2]);
		frustum[2][0] /= t;
		frustum[2][1] /= t;
		frustum[2][2] /= t;
		frustum[2][3] /= t;

		/* Extract the TOP plane */
		frustum[3][0] = clip[3] - clip[1];
		frustum[3][1] = clip[7] - clip[5];
		frustum[3][2] = clip[11] - clip[9];
		frustum[3][3] = clip[15] - clip[13];

		/* Normalize the result */
		t = sqrt(frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2]);
		frustum[3][0] /= t;
		frustum[3][1] /= t;
		frustum[3][2] /= t;
		frustum[3][3] /= t;

		/* Extract the FAR plane */
		frustum[4][0] = clip[3] - clip[2];
		frustum[4][1] = clip[7] - clip[6];
		frustum[4][2] = clip[11] - clip[10];
		frustum[4][3] = clip[15] - clip[14];

		/* Normalize the result */
		t = sqrt(frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2]);
		frustum[4][0] /= t;
		frustum[4][1] /= t;
		frustum[4][2] /= t;
		frustum[4][3] /= t;

		/* Extract the NEAR plane */
		frustum[5][0] = clip[3] + clip[2];
		frustum[5][1] = clip[7] + clip[6];
		frustum[5][2] = clip[11] + clip[10];
		frustum[5][3] = clip[15] + clip[14];

		/* Normalize the result */
		t = sqrt(frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2]);
		frustum[5][0] /= t;
		frustum[5][1] /= t;
		frustum[5][2] /= t;
		frustum[5][3] /= t;
	}

	bool NarwhalCameraV2::testPointInFrustum(glm::vec3 v)
	{
		for (int p = 0; p < 6; p++)
			if (frustum[p][0] * v.x + frustum[p][1] * v.y + frustum[p][2] * v.z + frustum[p][3] <= 0)
				return false;
		return true;
	}

	glm::vec3 NarwhalCameraV2::project(glm::vec3 pos3d, float window_width, float window_height)
	{
		glm::vec3 norm = viewprojection_matrix.project(pos3d); //returns from 0 to 1
		norm.x *= window_width;
		norm.y *= window_height;
		return norm;
	}

	glm::vec3 NarwhalCameraV2::unproject(glm::vec3 coord2d, float window_width, float window_height)
	{
		coord2d.x = (coord2d.x * 2.0f) / window_width - 1.0f;
		coord2d.y = (coord2d.y * 2.0f) / window_height - 1.0f;
		coord2d.z = 2.0f * coord2d.z - 1.0f;
		Matrix44 inv_vp = viewprojection_matrix;
		inv_vp.inverse();
		glm::vec4 r = inv_vp * glm::vec4(coord2d, 1.0f);
		return glm::vec3(r.x / r.w, r.y / r.w, r.z / r.w);
	}

	glm::vec3 NarwhalCameraV2::getRayDirection(int mouse_x, int mouse_y, float window_width, float window_height)
	{
		glm::vec3 mouse_pos((float)mouse_x, window_height - mouse_y, 1.0f);
		glm::vec3 p = unproject(mouse_pos, window_width, window_height);
		return glm::normalize(p - eye);
	}

	float NarwhalCameraV2::getProjectedScale(glm::vec3 pos3D, float radius) {
		
		float dist = glm::distance(eye, pos3D);
		return ((float)sin(fov * DEG2RAD) / dist) * radius * 200.0f; //100 is to compensate width in pixels
	}

	


	void NarwhalCameraV2::renderInMenu()
	{

		bool changed = false;
		changed |= ImGui::SliderFloat3("Eye", &eye.x, -100, 100);
		changed |= ImGui::SliderFloat3("Center", &center.x, -100, 100);
		changed |= ImGui::SliderFloat3("Up", &up.x, -100, 100);
		changed |= ImGui::SliderFloat("FOV", &fov, 15, 180);
		changed |= ImGui::SliderFloat("Near", &near_plane, 0.01, far_plane);
		changed |= ImGui::SliderFloat("Far", &far_plane, near_plane, 10000);
		if (changed)
			this->lookAt(eye, center, up);

	}


	

}

