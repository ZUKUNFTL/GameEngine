#pragma once
#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {

	class OpenGLVertexArray :public VertexArray 
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer> vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer> indexBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; };
	private:
		uint32_t m_RendererID;
		/*
		在OpenGL里，对VertexArray和VertexBuffer的理解：VertexArray有点像是VertexBuffer的parent容器，一个OpenGL里的VertexArray应该是有16个顶点属性的槽位，可以存放多个VertexBuffer的数据（这里应该存的是地址，不是真实复制了数据吧）。比如说，我可以有三个VBO，一个放顶点坐标，一个放normal，一个放纹理，最后都加入到VAO里，也是可以的。因此这里我们用数组去存
		*/
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}