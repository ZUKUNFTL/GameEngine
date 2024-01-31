#pragma once
#include "Hazel/Core/Base.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseCodes.h"

#include <glm/glm.hpp>
namespace Hazel{

	class  Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);

		static bool IsMouseButtonPressed(const MouseCode button);
		static glm::vec2 Input::GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}