#pragma once

#include <glm/glm.hpp>

//注意区分：Camera组件与Camera类，实体添加Camera组件、Camera组件拥有Camera类、所以实体能通过组件获得Camera

namespace Hazel {

	class Camera {

	public:
		Camera(const glm::mat4& projection)
			:m_Projection(projection){}

		const glm::mat4& GetProjection() const { return m_Projection; }
	private:
		glm::mat4 m_Projection;
	};
}