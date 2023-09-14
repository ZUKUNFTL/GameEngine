#pragma once

#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel {

	class HAZEL_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		//���������ջ����Ϊ�����һ����ʱ���� Attached �����ӣ�
		virtual void OnAttach() {}
		//���㱻�Ƴ�ʱ��Detach�����룩
		virtual void OnDetach() {}
		//OnUpdate �����ڲ����ʱ�� Application ���ã�Ӧ��ÿ֡����һ��
		virtual void OnUpdate() {}
		//OnEvent������õ��¼�ʱ�����Ǵ�������ա���Щ�����麯�������Կ����ڴ����Լ��Ĳ�ʱ override
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}