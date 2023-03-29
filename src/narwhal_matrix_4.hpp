#pragma once
//Credit to Javi Agenjo for the original code

#include <glm/glm.hpp>

#define DEG2RAD 0.0174532925
#define RAD2DEG 57.295779513

namespace narwhal {

	class Matrix44
	{
	public:
		static const Matrix44 IDENTITY;

		//This matrix works in 
		union { //allows to access the same var using different ways
			struct
			{
				float        _11, _12, _13, _14;
				float        _21, _22, _23, _24;
				float        _31, _32, _33, _34;
				float        _41, _42, _43, _44;
			};
			float M[4][4]; //[row][column]
			float m[16];
		};

		Matrix44();
		Matrix44(const float* v);

		void clear();
		void setIdentity();
		void transpose();
		void normalizeAxis();

		//get baseVectors
		glm::vec3 rightVector() { return glm::vec3(m[0], m[1], m[2]); }
		glm::vec3 topVector() { return glm::vec3(m[4], m[5], m[6]); }
		glm::vec3 frontVector() { return glm::vec3(m[8], m[9], m[10]); }

		bool inverse();
		void setUpAndOrthonormalize(glm::vec3 up);
		void setFrontAndOrthonormalize(glm::vec3 front);

		Matrix44 getRotationOnly(); //used when having scale

		//rotate only
		glm::vec3 rotateVector(const glm::vec3& v) const;

		//transform using local coordinates
		void translate(float x, float y, float z);
		void rotate(float angle_in_rad, const glm::vec3& axis);
		void scale(float x, float y, float z);

		//transform using global coordinates
		void translateGlobal(float x, float y, float z);
		void rotateGlobal(float angle_in_rad, const glm::vec3& axis);

		//create a transformation matrix from scratch
		void setTranslation(float x, float y, float z);
		void setTranslation(glm::vec3 v) { setTranslation(v.x, v.y, v.z); };
		void setRotation(float angle_in_rad, const glm::vec3& axis);
		void setScale(float x, float y, float z);


		glm::vec3 getTranslation();

		bool getXYZ(float* euler) const;

		void lookAt(glm::vec3& eye, glm::vec3& center, glm::vec3& up);
		void perspective(float fov, float aspect, float near_plane, float far_plane);
		void ortho(float left, float right, float bottom, float top, float near_plane, float far_plane);

		glm::vec3 project(const glm::vec3& v);

		Matrix44 operator * (const Matrix44& matrix) const;

	};
	//Operators, they are our friends
//Matrix44 operator * ( const Matrix44& a, const Matrix44& b );
	
	glm::vec3 operator * (const Matrix44& matrix, const glm::vec3& v);
	glm::vec4 operator * (const Matrix44& matrix, const glm::vec4& v);

}

