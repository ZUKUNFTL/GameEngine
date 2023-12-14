#pragma once

#include "entt.hpp"

#include "Hazel/Core/Timestep.h"
namespace Hazel {
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(TimeStep ts);

		Entity CreateEntity(const std::string& name = std::string());
	private:
		entt::registry m_Registry;

		//让Entity可以访问Scene的私有成员
		friend class Entity;
	};
}
