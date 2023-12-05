#pragma once
#include <Hazel.h>

#include "ParticleSystem.h"

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
		virtual void OnUpdate(Hazel::TimeStep ts) override;
		//Imgui图层渲染
		virtual void OnImGuiRender() override;
		//OnEvent，当层得到事件时，我们从这里接收。这些都是虚函数，所以可以在创建自己的层时 override
		virtual void OnEvent(Hazel::Event& event) override;
	private:
		Hazel::Ref<Shader> m_FlatColorShader;
		Hazel::Ref<VertexArray> m_SquareVA;

		Hazel::Ref<Texture2D> m_CheckerboardTexture;
		Hazel::Ref<Texture2D> m_SpriteSheet;
		Hazel::Ref<SubTexture2D> m_TextureStairs;
		Hazel::Ref<SubTexture2D> m_TetureBarrel;
		Hazel::Ref<SubTexture2D> m_TetureTree;
		Hazel::Ref<Framebuffer> m_Framebuffer;

		Hazel::OrthographicCameraController m_CameraController;

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		ParticleSystem m_ParticleSystem;
		ParticleProps m_Particle;

		uint32_t m_MapWidth, m_MapHeight;
		std::unordered_map<char, Hazel::Ref<SubTexture2D> > s_TextureMap;
	};

}
