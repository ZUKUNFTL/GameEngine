#pragma once

#include <memory>

#ifdef HZ_DEBUG
	#if defined(HZ_PLATFORM_WINDOWS)
		#define HZ_DEBUGBREAK() __debugbreak()
	#elif defined(HZ_PLATFORM_LINUX)
	#include <signal.h>
		#define HZ_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define HZ_ENABLE_ASSERTS
#else
	#define HZ_DEBUGBREAK()
#endif 

//旧的宏定义
//#ifdef HZ_PLATFORM_WINDOWS
//#if HZ_DYNAMIC_LINK
//#ifdef HZ_BUILD_DLL
//#define HAZEL_API __declspec(dllexport)
////#define IMGUI_API _declspec (dllexport) // 添加导出这一行不要了,在premake里面做
//#else
//#define HAZEL_API __declspec(dllimport)
////#define IMGUI_API __declspec (dllimport) // 添加导入
//#endif
//#else
//#define HAZEL_API 
//#endif
//#else
//#error Hazel only support Windows!
//#endif  
//
//#ifdef HZ_DEBUG
//	#define HZ_ENABLE_ASSERTS 
//#endif 
//
//#ifdef HZ_ENABLE_ASSERTS
//	//检查x是否成功，如果不成功则打印日志，并中断
//	#define HZ_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
//	#define HZ_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
//#else
//	//这种情况将不会进行检查
//	#define HZ_ASSERT(x, ...)
//	#define HZ_CORE_ASSERT(x, ...)
//#endif

/*
	HZ_EXPAND_MACRO:其目的是简化宏的展开。具体来说，它接受一个参数 x 并返回该参数，从而实现将参数展开的效果。
	在C语言的宏展开中，这样的宏通常用于避免不必要的额外级别的展开。在这里，HZ_EXPAND_MACRO 的作用是将传递给它的参数直接展开，以确保宏的正确使用。在宏中使用 HZ_EXPAND_MACRO 
	可以帮助处理由于宏展开规则而导致的潜在问题。
	HZ_STRINGIFY_MACRO:其目的是将传入的参数 x 转换为字符串文本。具体来说，#x 是预处理器的字符串化操作符，将参数转换为字符串。这种字符串化的操作通常在宏中使用，用于将参数的
	值转换为字符串，以便在日志、错误消息或其他地方使用。

*/

#define HZ_EXPAND_MACRO(x) x
#define HZ_STRINGIFY_MACRO(x) #x

//1向左移动x位。
#define BIT(x) (1 << x)

//#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }


namespace Hazel {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "Hazel/Core/Log.h"
#include "Hazel/Core/Assert.h"

