#pragma once

#include "entt.hpp"

#include "Hazel/Core/Timestep.h"
#include "Hazel/Renderer/EditorCamera.h"
namespace Hazel {
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdateRuntime(TimeStep ts);
		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		//改变窗口大小会触发
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		//记录当前的宽高比
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		//让Entity可以访问Scene的私有成员
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

}
