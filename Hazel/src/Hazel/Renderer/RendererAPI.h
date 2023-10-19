#pragma once
//RendererAPI类，除了要有标识当前使用的API类型的函数外，还需要有很多与平台无关的渲染的API，
//比如清空Buffer、根据Vertex Array进行调用DrawCall等函数，所以这里先把RendererAPI类丰富一下
#include <glm/glm.hpp>

#include "VertexArray.h"
namespace Hazel {

	class RendererAPI
	{
	public:
		// 渲染API的类型, 这一块应该由RendererAPI负责, 而不是Renderer负责
		enum class API
		{
			None = 0, OpenGL = 1
		};

	public:
		// 把相关代码抽象成以下三个接口，放在RenderAPI类里
		virtual void Clear() const = 0;
		virtual void SetClearColor(const glm::vec4&) const = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;
	
		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}