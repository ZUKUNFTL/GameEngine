#pragma once

namespace Hazel {

	class VertexBuffer 
	{
	public:
		virtual ~VertexBuffer(){}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		//这里之所以不在构造函数中进行传参，是因为我们需要更具不同的模式来创建实例
		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t size);
	};

}
