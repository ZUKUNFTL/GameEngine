#pragma once

#include <glm/glm.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"
namespace Hazel {

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string & tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4(1.0f);
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4 & transform)
			: Transform(transform) {}
		operator const glm::mat4& () { return Transform; }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f,1.0f };
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera  Camera;
		//是否是主摄像机的标志
		bool Primary = true;// TODO: think about moving to Scene
		//是否是可变宽高比的相机
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

	};

	//本地脚本组件
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		std::function<void()> InstantiateFunction;
		std::function<void()> DestroyInstanceFunction;

		std::function<void(ScriptableEntity*)> OnCreateFunction;
		std::function<void(ScriptableEntity*)> OnDestroyFunction;
		std::function<void(ScriptableEntity*, TimeStep)> OnUpdateFunction;
		template<typename T>
		void Bind()
		{
			// 这里绑定的函数功能是：根据T动态实例化Instanse
			InstantiateFunction = [&]() { Instance = new T(); };
			DestroyInstanceFunction = [&]() { delete (T*)Instance; Instance = nullptr; };

			// 这里是绑定T的函数
			OnCreateFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnCreate(); };
			OnDestroyFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnDestroy(); };
			OnUpdateFunction = [](ScriptableEntity* instance, TimeStep ts) { ((T*)instance)->OnUpdate(ts); };
		}

	};
}