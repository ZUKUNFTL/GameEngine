#pragma once
#include "Hazel/Layer.h"

#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/ApplicationEvent.h"
namespace Hazel {

	class HAZEL_API ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		//���������ջ����Ϊ�����һ����ʱ���� Attached �����ӣ�
		void OnAttach();
		//���㱻�Ƴ�ʱ��Detach�����룩
		void OnDetach();
		//OnUpdate �����ڲ����ʱ�� Application ���ã�Ӧ��ÿ֡����һ��
		void OnUpdate();
		//OnEvent������õ��¼�ʱ�����Ǵ�������ա���Щ�����麯�������Կ����ڴ����Լ��Ĳ�ʱ override
		void OnEvent(Event& event);

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizedEvent(WindowResizeEvent& e);
	private:
		float m_Time = 0.0f;
	};
}