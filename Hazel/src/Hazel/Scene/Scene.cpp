#include "hzpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"

#include "glm/glm.hpp"
#include "Hazel/Renderer/Renderer2D.h"
namespace Hazel {
	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}

	void Scene::OnUpdate(TimeStep ts)
	{
		// 更新脚本
		{
			//获取场景中所有包含脚本组件的实体，nsc是对应的组件
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					if (!nsc.Instance)
					{
						nsc.Instance =  nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity{ entity, this };

						nsc.Instance->OnCreate();
					}

					nsc.Instance->OnUpdate(ts);
				});
		}

		// 获取到主摄像机，并且获取到摄像机的位置，用来计算投影矩阵projection
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;

		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					//找到主摄像机
					mainCamera = &camera.Camera;
					cameraTransform = &transform.Transform;
					break;
				}
			}
		}

		if (mainCamera) {
			// 注意这，BeginScene中传入主摄像机的投影矩阵与主摄像机的transform矩阵
			//只渲染主摄像机的实体
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);
			// m_Registry获取既有Transform组件又有SpriteRenderer组件的实体
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuad(transform, sprite.Color);
			}
			Renderer2D::EndScene();
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		//每个实体都有个平移组件
		entity.AddComponent<TransformComponent>();
		//每个实体都有个标签组件
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}

	}

}