#pragma once

#include "entt.hpp"
#include "Hazel/Scene/Scene.h"

namespace Hazel {

	class Entity 
	{
	public:
		Entity() = default;
		Entity(const Entity& other) = default;
		Entity(entt::entity handle, Scene* scene);

		//typename...args可变参数
		template<typename T, typename...Args>
		T& AddComponent(Args&& ...args)
		{
			HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			//std::forward实现完美转发
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		//用于判空
		operator bool() const { return (uint32_t)m_EntityHandle >= 0; }
	private:
		entt::entity m_EntityHandle;
		Scene* m_Scene = nullptr;

	};
}
