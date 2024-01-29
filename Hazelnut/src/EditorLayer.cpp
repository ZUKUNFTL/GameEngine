#include "EditorLayer.h"
#include "imgui/imgui.h"
#include "Hazel/Scene/SceneSerializer.h"

#include "Hazel/Utils/PlatformUtils.h"

#include "ImGuizmo.h"

#include "Hazel/Math/Math.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel {
	/*
0不绘制
1是墙壁
2是草地
3是箱子
*/
	static const uint32_t s_MapWidth = 24;
	static const char* s_MapTiles =
		"wwwwwwwwwwwwwwwwwwwwwwww"
		"wwwwwwwwwwwwwwwwwwwwwwww"
		"wwwwwwdddddddddddwwwwwww"
		"wwwwwddddwwwwwwwddwwwwww"
		"wwwwwddddwwwwwwwwwddwwww"
		"wwwwddddwwwwwwwwwwwwddww"
		"wwwwddddwwwwwwwwwwwwwwdd"
		"wwwwddwwwwwwwwdddddddddw"
		"wwwwddwwwwwwwwdddddddddw"
		"wwwwddwwwwwwwwwwwwwwddww"
		"wwwwdddwwwwwwwwwwwddwwww"
		"wwwwwddwwwwwwwwwdddwwwww"
		"wwwwwwddwwwwwwwddwwwwwww"
		"wwwwwwwwwddddddwwwwwwwww"
		"wwwwwwwwwwwwwwwwwwwwwwww"
		"wwwwwwwwwwwwwwwwwwwwwwww"
		"wwwwwwwwwwwwwwwwwwwwwwww"
		"wwwwwwwwwwwwwwwwwwwwwwww"
		"wwwwwwwwwwwwwwwwwwwwwwww"
		"wwwwwwwwwwwwwwwwwwwwwwww"
		;


	EditorLayout::EditorLayout()
		: Layer("EditorLayout"), m_CameraController(1280.0f / 720.0f, true)
	{
	}
	void EditorLayout::OnAttach()
	{
		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		//读取命令行数据
		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(sceneFilePath);
		}

		//设置好编辑时摄像机参数：宽高比、视角角度、近、远
		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

#if 0
		// Entity
		auto square = m_ActiveScene->CreateEntity("Green Square");
		square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_SquareEntity = square;

		auto redSquare = m_ActiveScene->CreateEntity("Red Square");
		redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
		auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			virtual void OnCreate()override
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				//随机生成位置
				translation.x = rand() % 10 - 5.0f;
			}

			virtual void OnDestroy() override
			{
			}

			virtual void OnUpdate(TimeStep ts) override
			{
				// 获取当前挂载CameraController脚本的实体的TransformComponent组件
				auto& translation = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;

				if (Input::IsKeyPressed(KeyCode::A))
					translation.x -= speed * ts;
				if (Input::IsKeyPressed(KeyCode::D))
					translation.x += speed * ts;
				if (Input::IsKeyPressed(KeyCode::W))
					translation.y += speed * ts;
				if (Input::IsKeyPressed(KeyCode::S))
					translation.y -= speed * ts;
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();

#endif
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayout::OnDetach()
	{

	}

	void EditorLayout::OnUpdate(Timestep ts)
	{
		HZ_PROFILE_FUNCTION();

		// 改变了窗口大小
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			//调整缓冲帧大小与imgui一样
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			// 调整摄像机保持正确的**宽高比**
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Update
		//有焦点时才去更新相机
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(ts);

		// 不需要焦点，每一帧都需要刷新
		m_EditorCamera.OnUpdate(ts);

		// Render
		Renderer2D::ResetStats();
		//绑定自定义的帧缓冲（等于解绑到渲染到默认的帧缓冲中）
		m_Framebuffer->Bind();
		// 只需这几个API即可渲染图形，状态初始化代码都被设置在Renderer2D类中
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		// 用-1填充帧缓冲的第二个颜色缓冲区
		m_Framebuffer->ClearAttachment(1, -1);

		//更新场景
		m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

		// 1.获取当前鼠标距离整个屏幕左上角(0,0)的位置
		auto [mx, my] = ImGui::GetMousePos();
		// 2.鼠标绝对位置减去viewport窗口的左上角绝对位置=鼠标相对于viewport窗口左上角的位置
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		// 3.viewport窗口的右下角绝对位置-左上角的绝对位置=viewport窗口的大小
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		// 翻转y,使其左下角开始才是(0,0)，与纹理坐标相同
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			//HZ_CORE_WARN("Mouse xy = {0} {1}", mouseX, mouseY);
			// 4.读取帧缓冲第二个缓冲区的数据
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
			HZ_CORE_WARN("Pixel data = {0}", pixelData);
		}

		//解绑帧缓冲
		m_Framebuffer->Unbind();

	}

	void EditorLayout::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		// 完善UI：设置面板最小宽度
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		// 恢复
		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("Stats");

		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		//获取鼠标在viewport视口的相对位置
		// 1.先获取Viewport视口左上角与viewport视口标题栏距离的偏移位置（0,24)- 必须放这，因为标题栏后就是视口的左上角
		auto viewportOffset = ImGui::GetCursorPos();

		// 注意是获取Viewport视口下的bool值，来决定settings视口是否需要堵塞相应事件
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		//todo:暂时解决最小化后摄像机宽高比不同的Bug，因为窗口最小化事件与窗口重新调整大小不是同一个事件
		m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);


		// imgui渲染帧缓冲中的东西(附在帧缓冲上的颜色纹理缓冲)
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		/*
			imgui的uv默认是左下角为01，右下角为11，左上角为00，右上角是10

			ImVec2(0, 1):设置左上角点的uv是 0 1
			ImVec2(1, 0):设置右下角点的uv是 1 0
			因为我们绘制的quad的uv是左下角为00，右下角10，左上角01，右上角11。
		*/
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1,0 });


		// 2.获取vieport视口大小 - 包含标题栏的高
		auto windowSize = ImGui::GetWindowSize();
		// 3.获取当前vieport视口标题栏左上角距离当前整个屏幕左上角（0,0）的位置
		ImVec2 minBound = ImGui::GetWindowPos();
		// 4.计算viewport视口的左上角距离当前整个屏幕左上角（0,0）的位置
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;
		// 5. 计算viewport视口的右下角距离当前整个屏幕左上角（0,0）的位置
		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y - viewportOffset.y };
		// 6. 保存左上角和右下角距离整个屏幕左上角的位置
		m_ViewportBounds[0] = { minBound.x, minBound.y };
		m_ViewportBounds[1] = { maxBound.x, maxBound.y };

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			//使用透视投影
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			//设置焦点,viewport
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera
			//auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			//const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			////正交投影矩阵
			//const glm::mat4& cameraProjection = camera.GetProjection();
			////变化矩阵取逆矩阵即相机的视图矩阵
			//glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			// Editor camera
			// Camera - editor 编辑时的摄像机矩阵
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			// 这里可以说是传入相应参数，得到绘画出来的gizmos
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			// 如果gizmos被使用 或者 说被移动
			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				// 用增量旋转，解决矩阵可能会造成万向锁。
				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;// 每一帧增加没有限制角度，而不是固定在360度数。
				tc.Scale = scale;
			}
		}

		ImGui::PopStyleVar();
		ImGui::End();
		ImGui::End();
	}

	void EditorLayout::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayout::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(EditorLayout::OnMouseButtonPressed));
	}

	bool EditorLayout::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewScene();
			break;
		}
		case Key::O:
		{
			if (control)
				OpenScene();
			break;
		}
		case Key::S:
		{
			if (control && shift)
				SaveSceneAs();
			break;
		}

		// Gizmos
		case Key::Q:
			m_GizmoType = -1;
			break;
		case Key::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Key::R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
	}

	bool EditorLayout::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			/*
			  m_ViewportHovered 是为了在别的视口点击不会关闭当前显示的gizmo
			  后面两个&&是解决下面
			  1. 拖动gizmo移动一个实体与另一个实体重叠时停下，且另一个实体在当前实体上面，再点击gizmo想移动原先实体，那么会获取在上面另一个实体的实体ID，即另一个实体会被选中，会切换gizmo。
			  2. 显示了gizmo，但是若按下leftalt拖动旋转摄像机，则gizmo会消失
		    */

			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayout::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayout::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");
		if (!filepath.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayout::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}

}
