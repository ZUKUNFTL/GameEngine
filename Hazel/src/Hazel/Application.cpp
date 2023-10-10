#include "hzpch.h"
#include "Application.h"

#include "Hazel/Log.h"

#include <glad/glad.h>
#include "Hazel/Input.h"

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

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		float vertices[3 * 3] = {
			//逆时针
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		
		//glEnableVertexAttribArray(0)启用了顶点着色器的（location = 0）的属性变量
		glEnableVertexAttribArray(0);

		/*
		@brief
		使用glVertexAttribPointer函数可以告诉OpenGL该如何解析这些数据（应用到逐个顶点属性上）
		第一个参数指定我们要配置的顶点属性。即在顶点着色器中使用layout(location = 0)定义了position顶点属性的位置值(Location)，把顶点属性的位置值设置为0，因为这里我们希望把数据传递到这一个顶点属性中，所以这里我们传入0。
		第二个参数指定顶点属性的大小。在例子中，三维坐标是一个vec3，它由3个值组成，所以大小是3。
		第三个参数指定数据的类型，这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。
		第四个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。
		第五个参数叫做步长(Stride)，它告诉我们在连续的数据中，一个顶点的三维坐标到下一个顶点的三维坐标的间隔。由于下个顶点的三维坐标在3个float之后，我们把步长设置为3 * sizeof(float)。
		最后一个参数的类型是void*，所以需要我们进行这个奇怪的强制类型转换。它表示位置数据在所开辟的那片显存空间中起始位置的偏移量(Offset)。由于顶点的三维数据就在数组的开头，也就是那片显存空间中的开头，所以这里是 (void*) 0。
		*/
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		uint32_t indices[3] = { 0, 1, 2 };
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
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
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

			auto [x, y] = Input::GetMousePosition();
			//HZ_CORE_TRACE("{0}, {1}", x, y);
			//更新每个图层
			// Application并不应该知道调用的是哪个平台的window，Window的init操作放在Window::Create里面
			// 所以创建完window后，可以直接调用其loop开始渲染
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

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
