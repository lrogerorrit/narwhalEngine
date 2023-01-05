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
			glm::mat4 mat4();
			glm::mat3 normalMatrix();
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

