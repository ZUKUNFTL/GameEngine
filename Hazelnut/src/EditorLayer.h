#pragma once
#include <Hazel.h>

#include "ParticleSystem.h"
#include "Panels/SceneHierarchyPanel.h"

namespace Hazel {

	class EditorLayout :public Layer
	{
	public:
		EditorLayout();
		virtual ~EditorLayout() = default;
		//当层推入层栈，成为程序的一部分时，被 Attached （链接）
		virtual void OnAttach() override;
		//当层被移除时，Detach（分离）
		virtual void OnDetach() override;
		//OnUpdate 则是在层更新时由 Application 调用，应该每帧调用一次
		virtual void OnUpdate(TimeStep ts) override;
		//Imgui图层渲染
		virtual void OnImGuiRender() override;
		//OnEvent，当层得到事件时，我们从这里接收。这些都是虚函数，所以可以在创建自己的层时 override
		virtual void OnEvent(Event& event) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
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
		Entity m_SquareEntity;

		//主摄像机实体
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		OrthographicCameraController m_CameraController;

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		int m_GizmoType = -1;

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
	};

}
