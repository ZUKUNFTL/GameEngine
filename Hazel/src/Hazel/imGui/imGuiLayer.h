#pragma once
#include "Hazel/Layer.h"

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
		float m_Time = 0.0f;
	};
}