workspace "Hazel"
	architecture"x64"
	--启动项
	startproject "Sandbox"

	-- 这里release未必是发行版本，Dist才是完全的发行版本；release就是一个更快的Debug比如去掉一些日志啥的来测试。
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	} 

--buildcfg代表我们的是发布还是调试，system代表我们的系统是mac还是windows，archittecture代表我们是x64还是32的
--定义输出文件夹名称：Debug-windows-x86_64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Includir后期会加入多个路径因此我们用{}来表示。
IncludeDir = {}
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"
IncludeDir["Glad"] = "Hazel/vendor/Glad/include"
IncludeDir["ImGui"] = "Hazel/vendor/imgui"
IncludeDir["glm"] = "Hazel/vendor/glm"
IncludeDir["stb_image"] = "Hazel/vendor/stb_image"


--添加了glfw的premake文件
include "Hazel/vendor/GLFW"
include "Hazel/vendor/Glad"
include "Hazel/vendor/imgui"

--location代表我们相对路径的当前目录
--shadredlib代表我们是动态库
project "Hazel"
	location "Hazel"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	--staticruntime:on代表着当我们构建可执行文件时，它会链接到它自己的运行时库。
	--而我们在动态库中将它关闭的原因是动态链接dll文件时，dll文件需要它运行时的库，因为dll文件就像最终产品一样。
	--这因为如此，我们静态链接，我们每个dll都有不同版本的库，这是个大问题。
	staticruntime "on"

	--targetdir 二进制的输出文件夹目录，等于我们把所有东西都放在bin/debug-windows-x64/Hazel
	--objdir 中间文件的输出文件夹目录
	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")
	
	--pchsource对于vs来说是必须的尽管对我们来说不需要，在其他平台上，它会被忽略
	pchheader "hzpch.h"
	pchsource "Hazel/src/hzpch.cpp"

	--递归读取所有的cpp和h文件
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	--包含头文件路径
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		--添加glfw的头文件 
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}
		
	--静态链接
	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	--filter说明它只适用于所选的系统
	--staticruntim这和静态库链接相关
	filter "system:windows"
		systemversion "latest"
		--定义宏
		defines
		{
			"HZ_PLATFORM_WINDOWS",
			"HZ_BUILD_DLL",
			"IMGUI_API=__declspec(dllexport)",
			--"HZ_ENABLE_ASSERTS",
			--这个宏代表我们不包括任何打开的GLFW头文件，知道你引入opengl
			"GLFW_INCLUDE_NONE"
		}
	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		--symbols:on代表debug版本
		symbols "on"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		--optimize:on代表release版本
		optimize "on"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs
	{
		"Hazel/vendor/spdlog/include",
		"Hazel/src",
		"Hazel/vendor",
		"%{IncludeDir.glm}"
	}

	--需要链接的项目
	links
	{
		"Hazel"
	}

	filter "system:windows"
		systemversion "latest"
		
		defines
		{
			"HZ_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"