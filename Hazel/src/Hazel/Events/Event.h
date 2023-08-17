#pragma once

#include "Hazel/Core.h"

//����Ӧ��д��Ԥ����ͷ�ļ�����Ϊ�����Ǳ�׼��
#include <string>
#include <functional>
#include <fstream>

/*
	Hazel�е��¼�Ϊ�����¼�����Ϊ�����Ǵ�����Щ��갴���¼���ʱ�򣬿���ֱ����ջ�Ϲ����¼���Ȼ���������ûص�������	
	���Ե����Ǵ�������¼���ʱ�򣬻���ͣ���������¼�����˳�Ϊ�����¼���δ�����Դ�����������¼��������Ͼ��ǲ���
	��Щ��Ϣ����ĳ���ط����д洢�������������¼���Ȼ�����ÿ֡�����¼����С�Ȼ����Ⱥʹ������ǣ����������յ��¼�
	ʱ��������
*/
namespace Hazel {

	enum class EventType
	{
		//���е��¼����ͣ�ÿ���¼����ж�Ӧ�Ĵ���
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		//�¼�������
		//������������һ���¼����ڶ��������磬����������갴ť�¼����������¼�����갴ť�¼���������¼���
		//�������ڵ�һ�¼�������Ӧ�ö������������Ǵ���һ��λ���������Ǿ������ö��λ��
		//Ȼ�����ǿ��Լ򵥵��������ǿ���һ���¼�����ʲô���ͣ�����һ���¼�ʵ��������ʲô���͡�
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

	/*
		\#�Ĺ����ǽ������ĺ���������ַ�������������##�����������������һ��.
		�������ǲ���Ҫ�а����¼����ʵ�����鿴��ʲô���ͣ���Ϊ�����¼�����һ�������¼�������ʵ����ʲô��������ǽ�GetStaticType����Ϊ��̬������
		������֮������Ҫһ����Ա����GetEventType����������Ϊ������ֻ��һ��ָ���¼���event�������û�ָ��ʱ������ϣ���ܹ�������ʵ����ʲô���͡�
	*/
	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

	//�ú���ж�����ӷ��㣬���ǲ����ڸ������н���ʵ��
	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	//�¼��Ļ���
	class HAZEL_API Event
	{
		friend class EventDispatcher;
	public:
		//��ȡ�¼�����
		virtual EventType GetEventType() const = 0;
		//��ȡ����
		virtual const char* GetName() const = 0;
		//��ȡ�¼�����־
		virtual int GetCategoryFlags() const = 0;
		//tostringĬ�Ϸ����¼�����
		virtual std::string ToString() const { return GetName(); }

		//�ж����Ƿ������Ǹ������¼�
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		//�ж��¼��Ƿ񱻴���������������Ͳ��ᴫ����������
		bool m_Handled = false;
	};

	//�¼�������
	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			//���m_Event���¼�������ͬ��ִ�ж�Ӧ�ĺ���
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	//����<<���ţ���������־ģ��ִ��tostring����
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}