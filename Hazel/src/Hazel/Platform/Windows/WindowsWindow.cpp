#include "hzpch.h"
#include "WindowsWindow.h"

namespace Hazel {

	//我们只想在初始化窗口的时候初始化一次glfw,因此将它设置为静态
	static bool s_GLFWInitialized = false;

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		//设置属性
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		
		//日志记录
		HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		//这里我们只初始化一次glfw
		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			HZ_CORE_ASSERT(success, "Could not intialize GLFW!");

			s_GLFWInitialized = true;
		}
		
		//创建glfw窗口
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		//设置当前使用的数据指针
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	//关闭窗口
	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	//提取事件，交换缓冲区
	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		//交换间隔表示交换缓冲区之前等待的帧数，通常称为vsync。
		//默认情况下，交换间隔为0，但因为屏幕每秒只更新60-75次，所以大部分的画面不会被显示。
		//而且，缓冲区有可能在屏幕更新的中间交换，出现屏幕撕裂的情况。
		//所以，可以将该间隔设为1，即每帧更新一次。它可以设置为更高的值，但这可能导致输入延迟。
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}
