include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Hazel"
	architecture"x86_64"
	--启动项
	startproject "Hazelnut"

	-- 这里release未必是发行版本，Dist才是完全的发行版本；release就是一个更快的Debug比如去掉一些日志啥的来测试。
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	} 

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

--buildcfg代表我们的是发布还是调试，system代表我们的系统是mac还是windows，archittecture代表我们是x64还是32的
--定义输出文件夹名称：Debug-windows-x86_64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "Hazel/vendor/GLFW"
	include "Hazel/vendor/Glad"
	include "Hazel/vendor/imgui"
	include "Hazel/vendor/yaml-cpp"
	include "Hazel/vendor/box2d"
group ""

group "Core"
	include "Hazel"
	include "Hazel-ScriptCore"
group ""

group "Tools"
	include "Hazelnut"
group ""

group "Misc"
	include "Sandbox"
group ""
