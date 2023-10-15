#pragma once

#include "Core.h"

#include "Window.h"
#include "Hazel/LayerStack.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

#include "Hazel/imGui/imGuiLayer.h"

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/OrthographicCamera.h"
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
		ImGuiLayer* m_ImGuiLayer; //添加添加ImGUILayer
		bool m_Running = true;
		LayerStack m_LayerStack;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<VertexArray> m_SquareVA;
		std::shared_ptr<Shader> m_BlueShader;

		OrthographicCamera m_Camera;
	private:
		static Application* s_Instance;
	};
		//需要在客户端定义
	Application* CreateApplication();
}
