#include "hzpch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace Hazel {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		//这会以正确的方式给他上色，然后有时间戳%T，然后又log的名字%n，这里我们是client，然后有实际的日志消息%v%$
		spdlog::set_pattern("%^[%T] %n: %v%$");
		//创建一个日志实体
		s_CoreLogger = spdlog::stdout_color_mt("Hazel");
		//设置日志等级
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);

	}
}
