#pragma once

//主要用于添加C#调用C++脚本的内部调用类
namespace Hazel {

	class ScriptGlue
	{
	public:
		//注册组件
		static void RegisterComponents();
		static void RegisterFunctions();
	};
}