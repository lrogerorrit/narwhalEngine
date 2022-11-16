#pragma once

#include "narwhal_model.hpp"

//std
#include <memory>

namespace narwhal {

	struct Transform2dComponent{
			glm::vec2 translation{}; //(position,offset)
			glm::vec2 scale{ 1.f,1.f };
			float rotation;

			glm::mat2 mat2() {
				const float s = glm::sin(rotation);
				const float c = glm::cos(rotation);
				glm::mat2 rotMatrix{ {c,s},{-s,c} };
				
				glm::mat2 scaleMat{ {scale.x,0.f},{0.f,scale.y } };
				return rotMatrix*scaleMat;
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
		Transform2dComponent transform2d{};

	private:
		NarwhalGameObject(id_t objId) : id{ objId } {}
		id_t id;
	
	};

}

