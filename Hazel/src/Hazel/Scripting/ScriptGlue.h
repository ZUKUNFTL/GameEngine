#pragma once

//主要用于添加C#调用C++脚本的内部调用类
namespace Hazel {

	class ScriptGlue
	{
	public:
		static void RegisterFunctions();
	};
}