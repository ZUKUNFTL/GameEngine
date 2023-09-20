#pragma once

#include "Core.h"

#include "Window.h"
#include "Hazel/LayerStack.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
namespace Hazel {

	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverLayer(Layer* layer);

		bool OnWindowClose(WindowCloseEvent& e);

		inline Window& GetWindow() { return *m_Window; };
		//获取单例
		inline static Application& Get() { return *s_Instance; }
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};
		//需要在客户端定义
	Application* CreateApplication();
}



class Person
{
public:
	static Person* Get() {
		if (!s_Instence) {
			s_Instence = new Person();
			return s_Instence;
		}
	}
private:
	static Person* s_Instence;
};