#pragma once

#include "entt.hpp"

#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/EditorCamera.h"

class b2World;

namespace Hazel {
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		//复制场景
		static Ref<Scene> Copy(Ref<Scene> other);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name);

		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		//改变窗口大小会触发
		void OnViewportResize(uint32_t width, uint32_t height);

		//复制实体
		void DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		//记录当前的宽高比
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		//物理引擎
		b2World* m_PhysicsWorld = nullptr;

		//让Entity可以访问Scene的私有成员
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};

}
