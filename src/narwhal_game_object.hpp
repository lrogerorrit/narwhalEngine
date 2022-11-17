#pragma once

#include "narwhal_model.hpp"

//libs
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>

namespace narwhal {

	struct TransformComponent{
			glm::vec3 translation{}; //(position offset)
			glm::vec3 scale{ 1.f,1.f,1.f };
			glm::vec3 rotation{};

			// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
			// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
			// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
			//TODO: switch to quaternions
			glm::mat4 mat4() {
				const float c3 = glm::cos(rotation.z);
				const float s3 = glm::sin(rotation.z);
				const float c2 = glm::cos(rotation.x);
				const float s2 = glm::sin(rotation.x);
				const float c1 = glm::cos(rotation.y);
				const float s1 = glm::sin(rotation.y);
				return glm::mat4{
					{
						scale.x * (c1 * c3 + s1 * s2 * s3),
						scale.x * (c2 * s3),
						scale.x * (c1 * s2 * s3 - c3 * s1),
						0.0f,
					},
					{
						scale.y * (c3 * s1 * s2 - c1 * s3),
						scale.y * (c2 * c3),
						scale.y * (c1 * c3 * s2 + s1 * s3),
						0.0f,
					},
					{
						scale.z * (c2 * s1),
						scale.z * (-s2),
						scale.z * (c1 * c2),
						0.0f,
					},
					{translation.x, translation.y, translation.z, 1.0f} };
			}
		};
	
	class NarwhalGameObject
	{
	public:
		using id_t = unsigned int;

		static NarwhalGameObject createGameObject() {
			static id_t currentId = 0;
			return NarwhalGameObject{ currentId++ };
		}

		NarwhalGameObject(const NarwhalGameObject&) = delete;
		NarwhalGameObject& operator=(const NarwhalGameObject&) = delete;
		NarwhalGameObject(NarwhalGameObject&&) = default;
		NarwhalGameObject& operator=(NarwhalGameObject&&) = default;

		const id_t getId() const { return id; }

		std::shared_ptr<NarwhalModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		NarwhalGameObject(id_t objId) : id{ objId } {}
		id_t id;
	
	};

}

