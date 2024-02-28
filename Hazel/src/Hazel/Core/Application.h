#pragma once

#include "Base.h"

#include "Window.h"
#include "Hazel/Core/LayerStack.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

#include "Hazel/imGui/imGuiLayer.h"

#include "Hazel/Core/Timestep.h"
namespace Hazel {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;
		const char* operator[](int index) const 
		{
			HZ_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Game Engine Application"; // 默认运行程序名称
		std::string WorkingDirectory;				// 工作目录
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class  Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		void PushLayer(Layer* layer);
		void PushOverLayer(Layer* layer);

		bool OnWindowClose(WindowCloseEvent& e);

		bool OnWindowResize(WindowResizeEvent& e);

		inline Window& GetWindow() { return *m_Window; };

		void Close();
		//获取单例
		inline static Application& Get() { return *s_Instance; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }
	private:
		ApplicationSpecification m_Specification;
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer; //添加添加ImGUILayer
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};
		//需要在客户端定义
	Application* CreateApplication(ApplicationCommandLineArgs args);
}
