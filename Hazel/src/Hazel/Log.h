#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

//日志类
namespace Hazel{

	class HAZEL_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};	
}


//Core log macros
//HZ_CORE_ERROR(...)因为它是一个可变数量的参数,__VA_ARGS__ 是一个可变参数的宏.
#define  HZ_CORE_TRACE(...)   ::Hazel::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define  HZ_CORE_INFO(...)    ::Hazel::Log::GetCoreLogger()->info(__VA_ARGS__)
#define  HZ_CORE_WARN(...)    ::Hazel::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define  HZ_CORE_ERROR(...)   ::Hazel::Log::GetCoreLogger()->error(__VA_ARGS__)
#define  HZ_CORE_FATAL(...)   ::Hazel::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Client log macros
#define  HZ_TRACE(...)   ::Hazel::Log::GetClientLogger()->trace(__VA_ARGS__)
#define  HZ_INFO(...)    ::Hazel::Log::GetClientLogger()->info(__VA_ARGS__)
#define  HZ_WARN(...)    ::Hazel::Log::GetClientLogger()->warn(__VA_ARGS__)
#define  HZ_ERROR(...)   ::Hazel::Log::GetClientLogger()->error(__VA_ARGS__)
#define  HZ_FATAL(...)   ::Hazel::Log::GetClientLogger()->fatal(__VA_ARGS__)

//如果不想构建它可以这样做
//#define HZ_CORE_INFO 0