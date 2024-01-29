#include "hzpch.h"
#include "Application.h"

#include "Hazel/Core/Log.h"

#include "Hazel/Renderer/Renderer.h"

#include "Hazel/Core/Input.h"

#include <glfw/glfw3.h>

namespace Hazel {
	 
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	//单例
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, ApplicationCommandLineArgs args)
		:m_CommandLineArgs(args)
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

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
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

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

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		// 在这个函数中，调用OpenGLRendererAPI::SetViewport
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			//这里后期应该要用平台的公共接口来获取时间platform::gettime(),在windows中我们用GLFW的接口
			float time = (float)glfwGetTime();
			//计算delta time（每帧之间的间隔）
			Timestep Timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			
			auto [x, y] = Input::GetMousePosition();
			//HZ_CORE_TRACE("{0}, {1}", x, y);
			if (!m_Minimized)
			{
				//更新每个图层
				// Application并不应该知道调用的是哪个平台的window，Window的init操作放在Window::Create里面
				// 所以创建完window后，可以直接调用其loop开始渲染
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(Timestep);
			}

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
