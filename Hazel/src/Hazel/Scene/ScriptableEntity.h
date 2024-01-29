#pragma once

#include "Entity.h"

namespace Hazel {

	//脚本实体
	class ScriptableEntity 
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			// 根据Entity类找到关联的组件
			return m_Entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {}
	private:
		//真实实体
		Entity m_Entity;
		//让Scene可以访问到脚本实体的私有成员
		friend class Scene;
	};
}