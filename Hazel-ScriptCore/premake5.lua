﻿project "Hazel-ScriptCore"
	kind "SharedLib"
	language "C#"-- C#项目
	dotnetframework "4.7.2"

	-- 生成的目标位置
	targetdir ("%{wks.location}/Hazelnut/Resources/Scripts")
	objdir ("%{wks.location}/Hazelnut/Resources/Scripts/Intermediates")

	files 
	{
		"Source/**.cs",
		"Properties/**.cs"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
