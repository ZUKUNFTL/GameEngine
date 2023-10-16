#include "hzpch.h"
#include "Application.h"

#include "Hazel/Log.h"

#include "Hazel/Input.h"

#include <glfw/glfw3.h>

namespace Hazel {
	 
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	//单例
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		m_ImGuiLayer = new ImGuiLayer();
		PushOverLayer(m_ImGuiLayer);

		
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		HZ_CORE_INFO("{0}", e);

		//事件的处理顺序是从覆层开始处理
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			//如果事件被处理，我们将中断，不会再继续传播到其他图层
			if(e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverLayer(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			//计算delta time（每帧之间的间隔）
			float time = (float)glfwGetTime();
			TimeStep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			
			auto [x, y] = Input::GetMousePosition();
			//HZ_CORE_TRACE("{0}, {1}", x, y);
			//更新每个图层
			// Application并不应该知道调用的是哪个平台的window，Window的init操作放在Window::Create里面
			// 所以创建完window后，可以直接调用其loop开始渲染
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			//统一调用，调用了NewFrame
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				// 每一个Layer都在调用ImGuiRender函数
				layer->OnImGuiRender();
			//统一结束调用，调用了EndFrame
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}
}
