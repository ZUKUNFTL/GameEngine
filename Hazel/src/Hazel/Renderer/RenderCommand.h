#pragma once

#include "RendererAPI.h"

/*
可以看到，RenderCommand类只是把RendererAPI的内容，做了一个静态的封装，这样做是为了以后把函数加入到RenderCommandQueue里做的架构设计，也是为了后面的多线程渲染做铺垫。
RenderAPI和RenderCommand类有点难区分，RenderCommand类里面应该都是static函数，而且RendererAPI是一个虚基类，具体每个平台都有各自的PlatformRendererAPI类，而RenderCommand类里的static函数会直接调用RendererAPI类，也就是说，RenderCommand和Renderer类都是与Platform无关的，没有子类需要继承他们。
*/

namespace Hazel {

	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		// 注意RenderCommand里的函数都应该是单一功能的函数，不应该有其他耦合的任何功能
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			// 比如这里不可以调用vertexArray->Bind()函数
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}