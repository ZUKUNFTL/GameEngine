--location代表我们相对路径的当前目录
--shadredlib代表我们是动态库
project "Hazel"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	--staticruntime:on代表着当我们构建可执行文件时，它会链接到它自己的运行时库。
	--而我们在动态库中将它关闭的原因是动态链接dll文件时，dll文件需要它运行时的库，因为dll文件就像最终产品一样。
	--这因为如此，我们静态链接，我们每个dll都有不同版本的库，这是个大问题。
	staticruntime "off"

	--targetdir 二进制的输出文件夹目录，等于我们把所有东西都放在bin/debug-windows-x64/Hazel
	--objdir 中间文件的输出文件夹目录
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	--pchsource对于vs来说是必须的尽管对我们来说不需要，在其他平台上，它会被忽略
	pchheader "hzpch.h"
	pchsource "src/hzpch.cpp"

	--递归读取所有的cpp和h文件
	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",

		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	--包含头文件路径
	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.mono}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.Box2D}"
	}
		
	--静态链接
	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib",
		"Box2d",
		"%{Library.mono}"
	}

	filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

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

		links
		{
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.BCrypt}",
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		--symbols:on代表debug版本
		symbols "on"

		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		--optimize:on代表release版本
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}
