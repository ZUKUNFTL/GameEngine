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

		//��Entity���Է���Scene��˽�г�Ա
		friend class Entity;
	};
}
