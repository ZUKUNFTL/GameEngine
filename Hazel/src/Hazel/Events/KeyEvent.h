#pragma once
#include "Event.h"

namespace Hazel {
	class  KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

		//它既是按键事件，也是输入事件
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int keycode)
			: m_KeyCode(keycode) {}

		//按键的code
		int m_KeyCode;
	};

	class  KeyPressedEvent : public KeyEvent
	{
	public:
		//repeatCount：按键按下之后重复的次数 
		KeyPressedEvent(int keycode, bool isRepeat = false)
			: KeyEvent(keycode), m_IsRepeat(isRepeat) {}

		inline bool GetRepeatCount() const { return m_IsRepeat; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (repeat =" << m_IsRepeat << ")";
			return ss.str();
		}

		//通过宏来定义更加简单
		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool m_IsRepeat;
	};

	//键盘释放事件
	class  KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class  KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		//通过宏来定义更加简单
		EVENT_CLASS_TYPE(KeyTyped)
	};

}