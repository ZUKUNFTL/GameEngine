#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API __declspec(dllexport)
		//#define IMGUI_API _declspec (dllexport) // 添加导出这一行不要了,在premake里面做
	#else
		#define HAZEL_API __declspec(dllimport)
		//#define IMGUI_API __declspec (dllimport) // 添加导入
	#endif
#else
	#error Hazel only support Windows!
#endif 

#ifdef HZ_DEBUG
	#define HZ_ENABLE_ASSERTS 
#endif 

#ifdef HZ_ENABLE_ASSERTS
	//检查x是否成功，如果不成功则打印日志，并中断
	#define HZ_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define HZ_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	//这种情况将不会进行检查
	#define HZ_ASSERT(x, ...)
	#define HZ_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
