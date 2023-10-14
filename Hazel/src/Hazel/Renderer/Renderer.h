#pragma once
#include "RenderCommand.h"

//Renderer��һ���߼�����Ⱦ�࣬����������������͸߼��ṹ

namespace Hazel {

	class Renderer
	{
	public:
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		// TODO: δ�������Scene��������ز���������Camera��lighting, ��֤shaders�ܹ��õ���ȷ�Ļ�����ص�uniforms
		static void BeginScene();
		// TODO:
		static void EndScene();
		// TODO: ���VAOͨ��RenderCommand�µ�ָ����ݸ�RenderCommandQueue
		// Ŀǰ͵������ֱ�ӵ���RenderCommand::DrawIndexed()����
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
	};
}
