#pragma once

#include "narwhal_model.hpp"

//libs
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>
#include <unordered_map>

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

	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};
	
	class NarwhalGameObject
	{
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, NarwhalGameObject>;

		static NarwhalGameObject createGameObject() {
			static id_t currentId = 0;
			return NarwhalGameObject{ currentId++ };
		}

		static NarwhalGameObject makePointLight(float intensity = 1.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

		NarwhalGameObject(const NarwhalGameObject&) = delete;
		NarwhalGameObject& operator=(const NarwhalGameObject&) = delete;
		NarwhalGameObject(NarwhalGameObject&&) = default;
		NarwhalGameObject& operator=(NarwhalGameObject&&) = default;

		const id_t getId() const { return id; }

		glm::vec3 color{};
		TransformComponent transform{};

		// Optional Pointer Components
		std::shared_ptr<NarwhalModel> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr; 

	private:
		NarwhalGameObject(id_t objId) : id{ objId } {}
		id_t id;
	
	};

}

