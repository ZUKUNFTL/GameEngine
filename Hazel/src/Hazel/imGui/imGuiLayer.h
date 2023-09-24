#pragma once
#include "Hazel/Layer.h"

#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/ApplicationEvent.h"

/*
之前的ImGuiLayer是在SandboxApp.cpp里加入的，而实际上ImGui应该是游戏引擎自带的东西，
不应该是由用户定义添加到LayerStack里，所以需要为Application提供固有的ImGuiLayer成员，
可以用宏括起来，Release游戏的时候，就不用这个东西。
为了让每一个Layer都有一个ImGuiLayer，让每一个Layer都继承一个接口，用于绘制ImGui的内容，
同时让ImGuiLayer成为Hazel内在(intrinsic)的部分，需要在Application里面加上对应的LayerStack，
与其内部的Layer一一对应.
*/
namespace Hazel {

	class HAZEL_API ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		//当层推入层栈，成为程序的一部分时，被 Attached （链接）
		virtual void OnAttach() override;
		 //当层被移除时，Detach（分离）
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};
}