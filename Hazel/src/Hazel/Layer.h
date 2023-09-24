#pragma once

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

	class HAZEL_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		//当层推入层栈，成为程序的一部分时，被 Attached （链接）
		virtual void OnAttach() {}
		//当层被移除时，Detach（分离）
		virtual void OnDetach() {}
		//OnUpdate 则是在层更新时由 Application 调用，应该每帧调用一次
		virtual void OnUpdate() {}
		//Imgui图层渲染
		virtual void OnImGuiRender(){}
		//OnEvent，当层得到事件时，我们从这里接收。这些都是虚函数，所以可以在创建自己的层时 override
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}