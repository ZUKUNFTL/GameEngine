#pragma once
#include <Hazel.h>

#include "ParticleSystem.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
namespace Hazel {

	class EditorLayer :public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		//当层推入层栈，成为程序的一部分时，被 Attached （链接）
		virtual void OnAttach() override;
		//当层被移除时，Detach（分离）
		virtual void OnDetach() override;
		//OnUpdate 则是在层更新时由 Application 调用，应该每帧调用一次
		virtual void OnUpdate(Timestep ts) override;
		//Imgui图层渲染
		virtual void OnImGuiRender() override;
		//OnEvent，当层得到事件时，我们从这里接收。这些都是虚函数，所以可以在创建自己的层时 override
		virtual void OnEvent(Event& event) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		//这里这层主要渲染了可视化碰撞画面
		void OnOverlayRender();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		//序列化
		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		//播放
		void OnScenePlay();
		// 开始物理模拟模式
		void OnSceneSimulate();
		//暂停
		void OnSceneStop();

		//复制实体
		void OnDuplicateEntity();

		// 工具栏UI(播放按钮)
		void UI_Toolbar();
	private:
		Ref<Shader> m_FlatColorShader;
		Ref<VertexArray> m_SquareVA;

		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_SpriteSheet;
		Ref<SubTexture2D> m_TextureStairs;
		Ref<SubTexture2D> m_TetureBarrel;
		Ref<SubTexture2D> m_TetureTree;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;  //Hazel文件存储的路径

		Entity m_SquareEntity;

		//主摄像机实体
		Entity m_CameraEntity;
		Entity m_SecondCamera;
		//鼠标放置的实体
		Entity m_HoveredEntity;

		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		OrthographicCameraController m_CameraController;

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		int m_GizmoType = -1;

		bool m_ShowPhysicsColliders = false;

		ParticleSystem m_ParticleSystem;
		ParticleProps m_Particle;

		uint32_t m_MapWidth, m_MapHeight;
		std::unordered_map<char, Ref<SubTexture2D> > s_TextureMap;

		//imgui窗口大小
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		//保存Viewport窗口左上角和右下角距离整个屏幕左上角的位置
		glm::vec2 m_ViewportBounds[2];

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		// Hierarchy面板
		SceneHierarchyPanel m_SceneHierarchyPanel;
		//文件窗口面板
		ContentBrowserPanel m_ContentBrowserPanel;

		enum class SceneState
		{
			Edit=0, Play=1, Simulate = 2
		};

		SceneState m_SceneState = SceneState::Edit;

		//播放，暂停, 物理模拟模式按钮的纹理
		Ref<Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;
	};

}
