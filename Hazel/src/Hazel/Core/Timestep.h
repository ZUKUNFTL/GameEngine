#pragma once

namespace Hazel {

	//为了修复在OnUpdate中每帧移动旋转摄像机，由于不同电脑的刷新率不同，从而使得在不同配置下的机器显示的不一致。
	class TimeStep
	{
	public:
		TimeStep(float time = 0.0f)
			:m_Time(time)
		{
		}

		operator float() const { return m_Time; }

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.0f; }

	private:
		float m_Time;
			
	};
}
