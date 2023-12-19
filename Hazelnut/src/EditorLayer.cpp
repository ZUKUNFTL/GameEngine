#include "EditorLayer.h"
#include "imgui/imgui.h"

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
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_Framebuffer = Framebuffer::Create(fbSpec);

	m_ActiveScene = CreateRef<Scene>();
	// Entity
	auto square = m_ActiveScene->CreateEntity("Green Square");
	square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

	m_SquareEntity = square;


	m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
	m_CameraEntity.AddComponent<CameraComponent>();

	m_SecondCamera = m_ActiveScene->CreateEntity("Clip-Space Entity");
	auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
	cc.Primary = false;
}

void EditorLayout::OnDetach()
{

}

void EditorLayout::OnUpdate(TimeStep ts)
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

		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	// Update
	//有焦点时才去更新相机
	if (m_ViewportFocused)
		m_CameraController.OnUpdate(ts);

	// Render
	Renderer2D::ResetStats();
	//绑定自定义的帧缓冲（等于解绑到渲染到默认的帧缓冲中）
	m_Framebuffer->Bind();
		// 只需这几个API即可渲染图形，状态初始化代码都被设置在Renderer2D类中
	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();

	//更新场景
	m_ActiveScene->OnUpdate(ts);

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
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Exit")) Application::Get().Close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	ImGui::Begin("Settings");

	auto stats = Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	if (m_SquareEntity) {
		ImGui::Separator();
		auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("%s", tag.c_str());
		auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
		ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));	
		ImGui::Separator();
	}

	ImGui::DragFloat3("Camera Transform",
		glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3]));

	if (ImGui::Checkbox("Camera A", &m_PrimaryCamera))
	{
		m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
		m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
	}

	{
		auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
		float orthoSize = camera.GetOrthographicSize();
		if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
			camera.SetOrthographicSize(orthoSize);
	}

	ImGui::End();
		
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	// 注意是获取Viewport视口下的bool值，来决定settings视口是否需要堵塞相应事件
	m_ViewportFocused = ImGui::IsWindowFocused();
	m_ViewportHovered = ImGui::IsWindowHovered();
	Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

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
	ImGui::PopStyleVar();
	ImGui::End();
	ImGui::End();
}

void EditorLayout::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);
}

}
