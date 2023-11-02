#pragma once
#include <Hazel.h>

class Sandbox2D :public Hazel::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;
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
	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;

	Hazel::OrthographicCameraController m_CameraController;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};
