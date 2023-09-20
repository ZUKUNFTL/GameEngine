#pragma once
#include "hzpch.h"

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Hazel Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class HAZEL_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		//跟新我们窗口的缓冲区，输入事件等，它会在我们应用程序中一直调用
		virtual void OnUpdate() = 0;

		//返回窗口宽度
		virtual unsigned int GetWidth() const = 0;
		//返回窗口高度
		virtual unsigned int GetHeight() const = 0;
		//获取真实窗口
		virtual void* GetNativeWindow() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		//不同的平台需要去实现不同的create，macwindow,linuxwindow等，它们都继承于window
		//如果我们传入默认的结构体，则里面都是默认值
		static Window* Create(const WindowProps& props = WindowProps());
	};

}
