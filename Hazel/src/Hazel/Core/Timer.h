#pragma once

#include <chrono>

namespace Hazel {

	class Timer
	{
	public:
		Timer()
		{
			Reset();
		}

		void Timer::Reset()
		{
			m_Start = std::chrono::high_resolution_clock::now();
		}

		float Timer::Elapsed()
		{
			// 计算经过的时间（秒），以纳秒为单位，然后转换为秒
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
		}

		float Timer::ElapsedMillis()
		{
			// 获取经过的时间（毫秒），调用 Elapsed()，并将秒转换为毫秒
			return Elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};
}