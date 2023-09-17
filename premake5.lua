workspace "Hazel"
	architecture"x64"

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

--添加了glfw的premake文件
include "Hazel/vendor/GLFW"
include "Hazel/vendor/Glad"
include "Hazel/vendor/imgui"

--location代表我们相对路径的当前目录
--shadredlib代表我们是动态库
project "Hazel"
	location "Hazel"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

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
		"%{prj.name}/src/**.cpp"
	}
	--包含头文件路径
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		--添加glfw的头文件 
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
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
		cppdialect "C++17"
		systemversion "latest"
		--定义宏
		defines
		{
			"HZ_PLATFORM_WINDOWS",
			"HZ_BUILD_DLL",
			--"HZ_ENABLE_ASSERTS",
			--这个宏代表我们不包括任何打开的GLFW头文件，知道你引入opengl
			"GLFW_INCLUDE_NONE"
		}
		--在hazel中进入bin目录并复制hazel.dll到sandbox中
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		--buildoptions "/MDd"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		--buildoptions "/MD"
		runtime "Release"
		symbols "On"

	filter "configurations:Dist"
		defines "HZ_DIST"
		--buildoptions "/MD"
		runtime "Release"
		symbols "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

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
		"Hazel/src"
	}

	--需要链接的项目
	links
	{
		"Hazel"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "10.0"
		
		defines
		{
			"HZ_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		symbols "On"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		symbols "On"