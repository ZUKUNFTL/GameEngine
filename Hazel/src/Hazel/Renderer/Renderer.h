#pragma once
#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"
//Renderer是一个高级的渲染类，它将处理场景，网络和高级结构

namespace Hazel {

	class Renderer
	{
	public:
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		// TODO: 未来会接受Scene场景的相关参数，比如Camera、lighting, 保证shaders能够得到正确的环境相关的uniforms
		static void BeginScene(OrthographicCamera& camera);
		// TODO:
		static void EndScene();
		// TODO: 会把VAO通过RenderCommand下的指令，传递给RenderCommandQueue
		// 目前偷个懒，直接调用RenderCommand::DrawIndexed()函数
		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}
