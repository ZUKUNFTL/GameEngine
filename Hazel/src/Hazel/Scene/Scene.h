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
		//改变窗口大小会触发
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		entt::registry m_Registry;
		//记录当前的宽高比
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		//让Entity可以访问Scene的私有成员
		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}
