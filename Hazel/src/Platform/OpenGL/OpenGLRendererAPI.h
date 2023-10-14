#pragma once
#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel {

	class OpenGLRendererAPI :public RendererAPI
	{
	public:
		virtual void Clear() const;
		virtual void SetClearColor(const glm::vec4&) const;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray);

	};
}
