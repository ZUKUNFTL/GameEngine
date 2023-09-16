#pragma once
#include "Hazel/Layer.h"

namespace Hazel {

	class HAZEL_API ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		//当层推入层栈，成为程序的一部分时，被 Attached （链接）
		void OnAttach();
		//当层被移除时，Detach（分离）
		void OnDetach();
		//OnUpdate 则是在层更新时由 Application 调用，应该每帧调用一次
		void OnUpdate();
		//OnEvent，当层得到事件时，我们从这里接收。这些都是虚函数，所以可以在创建自己的层时 override
		void OnEvent(Event& event);
	private:
		float m_Time = 0.0f;
	};
}