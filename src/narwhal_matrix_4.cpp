//Credit to Javi Agenjo for the original code
#include "narwhal_matrix_4.hpp"

#include <algorithm>    // std::swap
#include <glm/gtc/type_ptr.hpp>

#define M_PI_2 1.57079632679489661923

namespace narwhal {
	const Matrix44 Matrix44::IDENTITY;

	Matrix44::Matrix44()
	{
		setIdentity();
	}

	Matrix44::Matrix44(const float* v)
	{
		memcpy((void*)m, (void*)v, sizeof(float) * 16);
	}

	Matrix44::Matrix44(glm::mat4& mat)
	{
		const float* matSource = (const float*)glm::value_ptr(mat);
		memcpy((void*)m, (void*)matSource, sizeof(float) * 16);
	}

	void Matrix44::clear()
	{
		memset(m, 0, 16 * sizeof(float));
	}

	void Matrix44::setIdentity()
	{
		m[0] = 1; m[4] = 0; m[8] = 0; m[12] = 0;
		m[1] = 0; m[5] = 1; m[9] = 0; m[13] = 0;
		m[2] = 0; m[6] = 0; m[10] = 1; m[14] = 0;
		m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
	}

	void Matrix44::transpose()
	{
		std::swap(m[1], m[4]); std::swap(m[2], m[8]); std::swap(m[3], m[12]);
		std::swap(m[6], m[9]); std::swap(m[7], m[13]); std::swap(m[11], m[14]);
	}

	glm::vec3 Matrix44::rotateVector(const glm::vec3& v) const
	{
		glm::vec4 v4 = (*this * glm::vec4(v ,0.0));
		return glm::vec3(v4.x, v4.y, v4.z);
	}

	void Matrix44::translateGlobal(float x, float y, float z)
	{
		Matrix44 T;
		T.setTranslation(x, y, z);
		*this = *this * T;
	}

	void Matrix44::rotateGlobal(float angle_in_rad, const glm::vec3& axis)
	{
		Matrix44 R;
		R.setRotation(angle_in_rad, axis);
		*this = *this * R;
	}

	void Matrix44::translate(float x, float y, float z)
	{
		Matrix44 T;
		T.setTranslation(x, y, z);
		*this = T * *this;
	}

	void Matrix44::rotate(float angle_in_rad, const glm::vec3& axis)
	{
		Matrix44 R;
		R.setRotation(angle_in_rad, axis);
		*this = R * *this;
	}

	void Matrix44::scale(float x, float y, float z)
	{
		Matrix44 S;
		S.setScale(x, y, z);
		*this = S * *this;
	}

	void Matrix44::setScale(float x, float y, float z)
	{
		setIdentity();
		m[0] = x;
		m[5] = y;
		m[10] = z;
	}

	//To create a traslation matrix
	void Matrix44::setTranslation(float x, float y, float z)
	{
		setIdentity();
		m[12] = x;
		m[13] = y;
		m[14] = z;
	}

	glm::vec3 Matrix44::getTranslation()
	{
		return glm::vec3(m[12], m[13], m[14]);
	}

	//To create a rotation matrix
	void Matrix44::setRotation(float angle_in_rad, const glm::vec3& axis)
	{
		clear();
		glm::vec3 axis_n = glm::normalize(axis);
		

		float c = cos(angle_in_rad);
		float s = sin(angle_in_rad);
		float t = 1 - c;

		M[0][0] = t * axis_n.x * axis_n.x + c;
		M[0][1] = t * axis_n.x * axis_n.y - s * axis_n.z;
		M[0][2] = t * axis_n.x * axis_n.z + s * axis_n.y;

		M[1][0] = t * axis_n.x * axis_n.y + s * axis_n.z;
		M[1][1] = t * axis_n.y * axis_n.y + c;
		M[1][2] = t * axis_n.y * axis_n.z - s * axis_n.x;

		M[2][0] = t * axis_n.x * axis_n.z - s * axis_n.y;
		M[2][1] = t * axis_n.y * axis_n.z + s * axis_n.x;
		M[2][2] = t * axis_n.z * axis_n.z + c;

		M[3][3] = 1.0f;
	}

	Matrix44 Matrix44::getRotationOnly()
	{
		Matrix44 trans = *this;
		trans.transpose();

		Matrix44 inv = *this;
		inv.inverse();

		return trans * inv;
	}
	bool Matrix44::getXYZ(float* euler) const
	{
		// Code adapted from www.geometrictools.com
		//	Matrix3<Real>::EulerResult Matrix3<Real>::ToEulerAnglesXYZ 
			// +-           -+   +-                                        -+
			// | r00 r01 r02 |   |  cy*cz           -cy*sz            sy    |
			// | r10 r11 r12 | = |  cz*sx*sy+cx*sz   cx*cz-sx*sy*sz  -cy*sx |
			// | r20 r21 r22 |   | -cx*cz*sy+sx*sz   cz*sx+cx*sy*sz   cx*cy |
			// +-           -+   +-                                        -+
		if (_13 < 1.0f)
		{
			if (_13 > -1.0f)
			{
				// y_angle = asin(r02)
				// x_angle = atan2(-r12,r22)
				// z_angle = atan2(-r01,r00)
				euler[1] = asinf(_13);
				euler[0] = atan2f(-_23, _33);
				euler[2] = atan2f(-_12, _11);
				return true;
			}
			else
			{
				// y_angle = -pi/2
				// z_angle - x_angle = atan2(r10,r11)
				// WARNING.  The solution is not unique.  Choosing z_angle = 0.
				euler[1] = (float)-M_PI_2;
				euler[0] = -atan2f(_21, _22);
				euler[2] = 0.0f;
				return false;
			}
		}
		else
		{
			// y_angle = +pi/2
			// z_angle + x_angle = atan2(r10,r11)
			// WARNING.  The solutions is not unique.  Choosing z_angle = 0.
			euler[1] = (float)M_PI_2;
			euler[0] = atan2f(_21, _22);
			euler[2] = 0.0f;
		}
		return false;
	}


	void Matrix44::lookAt(glm::vec3& eye, glm::vec3& center, glm::vec3& up)
	{
		glm::vec3 front = (center - eye);
		if (fabs(front.length()) <= 0.00001)
			return;

		front = glm::normalize(front);
		glm::vec3 right = glm::normalize(glm::cross(front,up));
		glm::vec3 top = glm::normalize(glm::cross(right,front));

		setIdentity();

		M[0][0] = right.x; M[0][1] = top.x; M[0][2] = -front.x;
		M[1][0] = right.y; M[1][1] = top.y; M[1][2] = -front.y;
		M[2][0] = right.z; M[2][1] = top.z; M[2][2] = -front.z;

		translate(-eye.x, -eye.y, -eye.z);
	}

	//double check this functions
	void Matrix44::perspective(float fov, float aspect, float near_plane, float far_plane)
	{
		setIdentity();

		float f = 1.0f / tan(fov * float(DEG2RAD) * 0.5f);

		M[0][0] = f / aspect;
		M[1][1] = f;
		M[2][2] = (far_plane + near_plane) / (near_plane - far_plane);
		M[2][3] = -1.0f;
		M[3][2] = 2.0f * (far_plane * near_plane) / (near_plane - far_plane);
		M[3][3] = 0.0f;
	}

	void Matrix44::ortho(float left, float right, float bottom, float top, float near_plane, float far_plane)
	{
		clear();
		M[0][0] = 2.0f / (right - left);
		M[3][0] = -(right + left) / (right - left);
		M[1][1] = 2.0f / (top - bottom);
		M[3][1] = -(top + bottom) / (top - bottom);
		M[2][2] = -2.0f / (far_plane - near_plane);
		M[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);
		M[3][3] = 1.0f;
	}

	//applies matrix projection to vector (returns in normalized coordinates)
	glm::vec3 Matrix44::project(const glm::vec3& v)
	{
		float x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12];
		float y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13];
		float z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14];
		float w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];

		return glm::vec3((x / w + 1.0f) / 2.0f, (y / w + 1.0f) / 2.0f, (z / w + 1.0f) / 2.0f);
	}


	//Multiply a matrix by another and returns the result
	Matrix44 Matrix44::operator*(const Matrix44& matrix) const
	{
		Matrix44 ret;

		unsigned int i, j, k;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				ret.M[i][j] = 0.0;
				for (k = 0; k < 4; k++)
					ret.M[i][j] += M[i][k] * matrix.M[k][j];
			}
		}

		return ret;
	}

	//Multiplies a vector by a matrix and returns the new vector
	glm::vec3 operator * (const Matrix44& matrix, const glm::vec3& v)
	{
		float x = matrix.m[0] * v.x + matrix.m[4] * v.y + matrix.m[8] * v.z + matrix.m[12];
		float y = matrix.m[1] * v.x + matrix.m[5] * v.y + matrix.m[9] * v.z + matrix.m[13];
		float z = matrix.m[2] * v.x + matrix.m[6] * v.y + matrix.m[10] * v.z + matrix.m[14];
		return glm::vec3(x, y, z);
	}

	//Multiplies a vector by a matrix and returns the new vector
	glm::vec4 operator * (const Matrix44& matrix, const glm::vec4& v)
	{
		float x = matrix.m[0] * v.x + matrix.m[4] * v.y + matrix.m[8] * v.z + v.w * matrix.m[12];
		float y = matrix.m[1] * v.x + matrix.m[5] * v.y + matrix.m[9] * v.z + v.w * matrix.m[13];
		float z = matrix.m[2] * v.x + matrix.m[6] * v.y + matrix.m[10] * v.z + v.w * matrix.m[14];
		float w = matrix.m[3] * v.x + matrix.m[7] * v.y + matrix.m[11] * v.z + v.w * matrix.m[15];
		return glm::vec4(x, y, z, w);
	}

	void Matrix44::setUpAndOrthonormalize(glm::vec3 up)
	{
		up= glm::normalize(up);

		//put the up vector in the matrix
		m[4] = up.x;
		m[5] = up.y;
		m[6] = up.z;

		//orthonormalize
		glm::vec3 right, front;
		right = rightVector();
		
		if (abs(glm::dot(right, up)) < 0.99998)
		{
			right = glm::cross(up, frontVector());
			front = glm::cross(right, up);
		}
		else
		{
			front = glm::cross(right, up);
			right = glm::cross(up ,front);
		}

		right= glm::normalize(right);
		front= glm::normalize(front);

		

		m[8] = front.x;
		m[9] = front.y;
		m[10] = front.z;

		m[0] = right.x;
		m[1] = right.y;
		m[2] = right.z;
	}

	void Matrix44::setFrontAndOrthonormalize(glm::vec3 front)
	{
		front= glm::normalize(front);
		

		//put the up vector in the matrix
		m[8] = front.x;
		m[9] = front.y;
		m[10] = front.z;

		//orthonormalize
		glm::vec3 right, up;
		right = rightVector();
		
		if (abs(glm::dot(right, front)) < 0.99998)
		{
			right= glm::cross(topVector(),front);
			up= glm::cross(front,right);
		}
		else
		{
			up = glm::cross(front, right);
			right = glm::cross(up,front);
			
		}

		right= glm::normalize(right);
		up= glm::normalize(up);

		m[4] = up.x;
		m[5] = up.y;
		m[6] = up.z;

		m[0] = right.x;
		m[1] = right.y;
		m[2] = right.z;

	}

	bool Matrix44::inverse()
	{
		unsigned int i, j, k, swap;
		float t;
		Matrix44 temp, final;
		final.setIdentity();

		temp = (*this);

		unsigned int m, n;
		m = n = 4;

		for (i = 0; i < m; i++)
		{
			// Look for largest element in column

			swap = i;
			for (j = i + 1; j < m; j++)// m or n
			{
				if (fabs(temp.M[j][i]) > fabs(temp.M[swap][i]))
					swap = j;
			}

			if (swap != i)
			{
				// Swap rows.
				for (k = 0; k < n; k++)
				{
					std::swap(temp.M[i][k], temp.M[swap][k]);
					std::swap(final.M[i][k], final.M[swap][k]);
				}
			}

			// No non-zero pivot.  The CMatrix is singular, which shouldn't
			// happen.  This means the user gave us a bad CMatrix.


			#define MATRIX_SINGULAR_THRESHOLD 0.00001 //change this if you experience problems with matrices

				if (fabsf(temp.M[i][i]) <= MATRIX_SINGULAR_THRESHOLD)
				{
					final.setIdentity();
					return false;
				}
			#undef MATRIX_SINGULAR_THRESHOLD

			t = 1.0f / temp.M[i][i];

			for (k = 0; k < n; k++)//m or n
			{
				temp.M[i][k] *= t;
				final.M[i][k] *= t;
			}

			for (j = 0; j < m; j++) // m or n
			{
				if (j != i)
				{
					t = temp.M[j][i];
					for (k = 0; k < n; k++)//m or n
					{
						temp.M[j][k] -= (temp.M[i][k] * t);
						final.M[j][k] -= (final.M[i][k] * t);
					}
				}
			}
		}

		*this = final;

		return true;
	}
}