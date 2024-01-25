#pragma once
#include "Hazel/Core/Base.h"
#include "Hazel/Core/KeyCodes.h"
namespace Hazel{

	class  Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);

		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}