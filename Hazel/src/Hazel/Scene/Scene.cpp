#include "hzpch.h"
#include "Scene.h"

#include "Components.h"
#include "ScriptableEntity.h"
#include "Hazel/Scripting/ScriptEngine.h"
#include "Entity.h"
#include "glm/glm.hpp"
#include "Hazel/Renderer/Renderer2D.h"

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace Hazel {

	static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Hazel::Rigidbody2DComponent::BodyType::Static:	return b2_staticBody;
		case Hazel::Rigidbody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
		case Hazel::Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		HZ_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
		delete m_PhysicsWorld;
	}

	// 为复制场景的实体的组件的辅助方法
	// 这里会接收到传过来的模板参数包（也就是componentGroup）
	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		// 这个lambda会递归调用
		// 隐式引用捕获dst、src或者"解开的Component包引用"，下面的Component是指具体的单个组件
		([&]()
			{
				auto view = src.view<Component>();
				// 遍历旧场景所有uuid组件的旧实体
				for (auto srcEntity : view)
				{
					// ** 旧实体的uuid - map - 对应新实体 * *
					entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);
					//获取旧实体的组件
					auto& srcComponent = src.get<Component>(srcEntity);
					//然后用API，** 复制旧实体的组件给新实体**
					dst.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			}(), ...);// 折叠表达式解包,每次lanmda会捕获第一个参数赋予component，然后再继续调用lamda进行捕获
		#if OLD_PATH
		auto view = src.view<Component>();
		// 2.1遍历旧场景所有uuid组件的旧实体
		for (auto e : view) {
			UUID uuid = src.get<IDComponent>(e).ID;
			HZ_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
			// 2.2用** 旧实体的uuid - map - 对应新实体 * *
			entt::entity dstEnttID = enttMap.at(uuid);
			// 3.1获取旧实体的组件
			auto& component = src.get<Component>(e);
			// 3.2然后用API，** 复制旧实体的组件给新实体**
			dst.emplace_or_replace<Component>(dstEnttID, component);// 添加或替换，保险。组件里面的数据自然会被拷贝
		}
		#endif
	}
	// 参数包转发
	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap) {
		// 推断出来的模板参数包转发，传递
		// <Component...> = <TransformComponent, SpriteRendererComponent,CircleRendererComponent>……
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.HasComponent<Component>())
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			}(), ...);
	}
	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		// 1.1创建新场景
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportHeight = other->m_ViewportHeight;
		newScene->m_ViewportWidth = other->m_ViewportWidth;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// 在新的场景创建实体
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			// 1.2为新场景创建和旧场景同名和uuid的实体
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name= srcSceneRegistry.get<TagComponent>(e).Tag;
			// 1.3并用**map存入（旧实体的uuid对应新实体）的关系**
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
		return newScene;
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// 更新脚本
		{
			// C# 实体脚本更新
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnUpdateEntity(entity, ts);
			}

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

		//物理
		{
			// script脚本影响Physics变化再当前帧渲染出来
			// 迭代速度：使用更少的迭代可以提高性能，但准确性会受到影响。使用更多迭代会降低性能但会提高模拟质量
			// Cherno说迭代速度，多久进行一次计算模拟。好奇这个6，是时间单位吗，毫秒？
			const int32_t velocityIterations = 6;// 这些参数应该移到编辑器
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{ 
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				// 获取物理模拟计算后的主体
				b2Body* body =(b2Body*)rb2d.RuntimeBody;
				// 将计算后的值赋予实体
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();// 获取z轴角度
			}

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
					cameraTransform = &transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera) {

			// 注意这，BeginScene中传入主摄像机的投影矩阵与主摄像机的transform矩阵
			//只渲染主摄像机的实体
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);
			//画长方形
			{
				// m_Registry获取既有Transform组件又有SpriteRenderer组件的实体
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group) {
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			}

			// 画圆
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		RenderScene(camera);
	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		// Physics
		{
			// 先script脚本影响Physics变化再当前帧渲染出来
			// 迭代速度：使用更少的迭代可以提高性能，但准确性会受到影响。使用更多迭代会降低性能但会提高模拟质量
			// 有点不董。。。。说啥：时间步长和迭代次数完全无关。迭代不是子步骤
			// Cherno说迭代速度，多久进行一次计算模拟。好奇这个6，是多少毫秒计算6次吗？
			const int32_t velocityIterations = 6;// 这些参数应该移到编辑器
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view) {
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				// 获取物理模拟计算后的主体
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				// 将计算后的值赋予实体
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();// 获取z轴角度
			}
			// 脚本影响Pyhsics再下面渲染出来
		}

		// Render 渲染场景
		RenderScene(camera);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		// 创建新的uuid
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		//每个实体都有个平移组件
		entity.AddComponent<TransformComponent>();
		//每个实体都有个标签组件
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		//添加映射
		m_EntityMap[uuid] = entity;
		return entity;
	}


	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		OnPhysics2DStart();
		// 脚本
		{
			ScriptEngine::OnRuntimeStart(this);
			// Instantiate all script entities

			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		OnPhysics2DStop();

		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
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

	Hazel::Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		// 1.创建旧实体同名的新实体
		Entity newEntity = CreateEntity(entity.GetName());
		// 2.复制组件
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
	}

	Hazel::Entity Scene::GetEntityByUUID(UUID uuid)
	{
		// TODO(Yan): Maybe should be assert
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };

		return {};
	}

	void Scene::OnPhysics2DStart()
	{
		// 1.创建一个物理世界
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });// 重力加速度向下

		// 1.1为当前场景所有具有物理组件的实体创建b2Body
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			// 2.1 主体定义用来指定动态类型和位移旋转参数
			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			// 2.2 由b2BodyDef创建主体
			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);// 是否固定旋转
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
				// 3.1定义盒子包围盒
				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0);
				// 3.2 b2FixtureDef用于定义一个物体的 fixture（夹具）属性。在物理引擎中，夹具是用来描述物体形状、密度、摩擦力等物理属性的对象。
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		//画长方形
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				//Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f), (int)entity);
			}
		}

		// 画圆
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		//Renderer2D::DrawLine(glm::vec3(1.0f), glm::vec3(5.0f),glm::vec4(1,0,1,1));

		Renderer2D::EndScene();
	}

	template<typename T>
	void Hazel::Scene::OnComponentAdded(Entity entity, T& component)
	{
		//静态断言：false，代表编译时断言，表达式为false，在编译时显示给定的出错信息。
		static_assert(false);
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

}