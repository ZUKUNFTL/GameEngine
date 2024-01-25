#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Core/Log.h"
#include <filesystem>

#ifdef HZ_ENABLE_ASSERTS

/*
	HZ_INTERNAL_ASSERT_IMPL: 这是断言的实现宏。当条件 check 不成立时，它使用 HZ##type##ERROR 打印错误消息，并调用 HZ_DEBUGBREAK() 来中断程序执行。msg 是断言失败时的错误消息，而 __VA_ARGS__ 允许传递额外的参数。
	HZ_INTERNAL_ASSERT_WITH_MSG: 提供了带有自定义消息的断言实现，消息格式为 "Assertion failed: {0}"。
	HZ_INTERNAL_ASSERT_NO_MSG: 提供了不带自定义消息的断言实现，消息格式为 "Assertion '{0}' failed at {1}:{2}"，包括条件、文件名和行号。

	HZ_INTERNAL_ASSERT_GET_MACRO_NAME:这个宏实际上是一个条件宏，根据传递的参数选择 macro。具体来说，如果有足够的参数（arg1、arg2 都存在），则选择 macro。这里的目的是根据用
	户提供的参数数量来确定使用带消息还是不带消息的宏。在这里它用于选择使用带有自定义消息的断言宏 HZ_INTERNAL_ASSERT_WITH_MSG 或不带自定义消息的断言宏 HZ_INTERNAL_ASSERT_NO_MSG。

	HZ_ASSERT: 这是用户在代码中使用的宏名称，表示一个断言。
	...: 这个宏使用了可变参数，允许用户传递不定数量的参数。
	HZ_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__): 这一部分通过 HZ_INTERNAL_ASSERT_GET_MACRO 宏来获取适当的断言宏。__VA_ARGS__ 是用户传递给 HZ_ASSERT 宏的参数，可以包括条件和可选的自定义消息。
	(_): 这里传递了一个占位符 _ 作为第一个参数，用于指示断言宏中的 type 参数为空（对应 HZ_INTERNAL_ASSERT_IMPL(type, check, msg, ...))。
	HZ_EXPAND_MACRO(...): 这是一个宏，用于展开参数。在这里，它将 HZ_INTERNAL_ASSERT_GET_MACRO 的结果展开，以便正确使用用户提供的参数。
*/


	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	//定义了断言的实现，根据不同情况生成错误消息并调用调试断点。
	#define HZ_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { HZ##type##ERROR(msg, __VA_ARGS__); HZ_DEBUGBREAK(); } }
	//提供了断言失败时的消息，用于指定断言失败时的自定义消息。
	#define HZ_INTERNAL_ASSERT_WITH_MSG(type, check, ...) HZ_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	//提供了默认的断言失败消息，包括条件、文件名和行号。
	#define HZ_INTERNAL_ASSERT_NO_MSG(type, check) HZ_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", HZ_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
	
	//根据传入的参数，选择使用带消息或不带消息的宏。
	#define HZ_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	//获取适当的宏，根据是否提供了自定义消息。
	#define HZ_INTERNAL_ASSERT_GET_MACRO(...) HZ_EXPAND_MACRO( HZ_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, HZ_INTERNAL_ASSERT_WITH_MSG, HZ_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define HZ_ASSERT(...) HZ_EXPAND_MACRO( HZ_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define HZ_CORE_ASSERT(...) HZ_EXPAND_MACRO( HZ_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define HZ_ASSERT(...)
	#define HZ_CORE_ASSERT(...)
#endif
