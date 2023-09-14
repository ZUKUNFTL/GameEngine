#pragma once
#include "hzpch.h"
#include "Hazel/Core.h"

/*
	Hazel中的事件为阻塞事件，因为当我们处理这些鼠标按下事件的时候，可能直接在栈上构造事件，然后立即调用回调函数。	
	所以当我们处理这个事件的时候，会暂停所有其他事件。因此称为阻塞事件。未来可以创建带缓冲的事件，基本上就是捕获
	这些信息，在某个地方队列存储，不阻塞其他事件，然后可能每帧遍历事件队列。然后调度和处理它们，而不是在收到事件
	时立即处理。
*/
namespace Hazel {

	enum class EventType
	{
		//所有的事件类型，每个事件都有对应的代码
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		//事件过滤器
		//我们有能力让一个事件属于多个类别，例如，键盘鼠标和鼠标按钮事件都是输入事件，鼠标按钮事件就是鼠标事件。
		//我们想在单一事件类型上应用多个类别，所以我们创建一个位域，这样我们就能设置多个位，
		//然后我们可以简单的屏蔽它们看看一个事件属于什么类型，或者一个事件实际上属于什么类型。
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

	/*
		\#的功能是将其后面的宏参数进行字符串化操作，用##把两个宏参数贴合在一起.
		这里我们不需要有按键事件类的实例来查看是什么类型，因为按键事件总是一个按键事件，不管实例是什么，因此我们将GetStaticType声明为静态变量。
		而我们之所以需要一个成员函数GetEventType（），是因为当我们只是一个指向事件（event）的引用或指针时，我们希望能够看到它实际是什么类型。
	*/
	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

	//用宏进行定义更加方便，我们不用在各自类中进行实现
	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	//事件的基类
	class HAZEL_API Event
	{
	public:
		//判断事件是否被处理，如果被处理它就不会传播到其他层
		bool Handled = false;

		//获取事件类型
		virtual EventType GetEventType() const = 0;
		//获取命名
		virtual const char* GetName() const = 0;
		//获取事件类别标志
		virtual int GetCategoryFlags() const = 0;
		//tostring默认返回事件名称（用于日志记录用）
		virtual std::string ToString() const { return GetName(); }

		//判断它是否属于那个类别的事件
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

	};

	//事件调度器
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
			//如果m_Event的事件类型相同则执行对应的函数
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	//重载<<符号，可以让日志模块执行tostring函数
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}