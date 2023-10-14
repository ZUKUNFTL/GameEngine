#pragma once
#include "RenderCommand.h"

//Renderer是一个高级的渲染类，它将处理场景，网络和高级结构

namespace Hazel {

	class Renderer
	{
	public:
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		// TODO: 未来会接受Scene场景的相关参数，比如Camera、lighting, 保证shaders能够得到正确的环境相关的uniforms
		static void BeginScene();
		// TODO:
		static void EndScene();
		// TODO: 会把VAO通过RenderCommand下的指令，传递给RenderCommandQueue
		// 目前偷个懒，直接调用RenderCommand::DrawIndexed()函数
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
	};
}
