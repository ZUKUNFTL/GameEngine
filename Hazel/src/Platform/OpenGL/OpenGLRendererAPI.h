#pragma once
#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel {

	class OpenGLRendererAPI :public RendererAPI
	{
	public:

		virtual void Init();
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		virtual void Clear();
		virtual void SetClearColor(const glm::vec4&);
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0);
	};
}
