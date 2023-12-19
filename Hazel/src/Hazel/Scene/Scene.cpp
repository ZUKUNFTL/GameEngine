#include "hzpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"

#include "glm/glm.hpp"
#include "Hazel/Renderer/Renderer2D.h"
namespace Hazel {
	static void DoMath(const glm::mat4& transform) {

	}
	//当实体添加TransformComponent组件时执行OnTransformConstruct方法
	static void OnTransformConstruct(entt::registry& registry, entt::entity entity) {

	}
	Scene::Scene()
	{
#if ENTT_EXAMPLE_CODE
		// 1.创建一个实体,返回整数 id
		entt::entity entity = m_Registry.create(); // 是uint32_t
		// 2.实体放入TransformComponent组件.arg1:id，arg2：glm会转换为TransformComponent
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		// 3.判断实体是否有哦TransformComponent组件
		//if (m_Registry.has<TransformComponent>(entity))  // 代码报错不知道为什么
		// 4.获取实体的TransformComponent组件,根据id
		TransformComponent& transformCom = m_Registry.get<TransformComponent>(entity);
		// 5.m_Registry已注册的所有TransformComponent组件
		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view) {
			TransformComponent& transform1 = m_Registry.get<TransformComponent>(entity); // view、m_Registry两个都行
			TransformComponent& transform2 = view.get<TransformComponent>(entity);
		}
		// 6.m_Registry获取既有Transform组件又有Mesh组件的实体
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group) {
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			//Renderer::Submit(mesh, transform);
		}
		// 7.设置当实体添加TransformComponent组件时执行OnTransformConstruct方法
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();
#endif
	}
	Scene::~Scene()
	{
	}

	void Scene::OnUpdate(TimeStep ts)
	{
		// 获取到主摄像机，并且获取到摄像机的位置，用来计算投影矩阵projection
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;

		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

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
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

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