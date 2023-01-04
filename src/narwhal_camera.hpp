#pragma once

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace narwhal {
	class NarwhalCamera {
	public:
		

		void setOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		
		void setViewYXZ(glm::vec3 position,glm::vec3 rotation);
		void setViewYXZ(glm::vec3 position, float xRot, float yRot, float zRot) {
			this->setViewYXZ(position, glm::vec3{ xRot, yRot, zRot });
		}

		const glm::mat4& getProjection() const { return projectionMatrix; }
		const glm::mat4& getView() const { return viewMatrix; }

	private:
		glm::mat4 projectionMatrix{ 1.f };
		glm::mat4 viewMatrix{ 1.f };
		
		
	};
}