#include "hzpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>
namespace Hazel {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Hazel::ShaderDataType::Float:    return GL_FLOAT;
		case Hazel::ShaderDataType::Float2:   return GL_FLOAT;
		case Hazel::ShaderDataType::Float3:   return GL_FLOAT;
		case Hazel::ShaderDataType::Float4:   return GL_FLOAT;
		case Hazel::ShaderDataType::Mat3:     return GL_FLOAT;
		case Hazel::ShaderDataType::Mat4:     return GL_FLOAT;
		case Hazel::ShaderDataType::Int:      return GL_INT;
		case Hazel::ShaderDataType::Int2:     return GL_INT;
		case Hazel::ShaderDataType::Int3:     return GL_INT;
		case Hazel::ShaderDataType::Int4:     return GL_INT;
		case Hazel::ShaderDataType::Bool:     return GL_BOOL;
		}

		HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> vertexBuffer)
	{
		HZ_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{

			//glEnableVertexAttribArray(0)启用了顶点着色器的（location = 0）的属性变量
			glEnableVertexAttribArray(index);

			/*
			@brief
			使用glVertexAttribPointer函数可以告诉OpenGL该如何解析这些数据（应用到逐个顶点属性上），它存储在VAO中，当我们换了一个缓冲区VBO时，我们不需要重复调用它，只需要绑定你想要的VAO就行。同时它是直接绑定我们的shader
			第一个参数指定我们要配置的顶点属性。即在顶点着色器中使用layout(location = 0)定义了position顶点属性的位置值(Location)，把顶点属性的位置值设置为0，因为这里我们希望把数据传递到这一个顶点属性中，所以这里我们传入0。
			第二个参数指定顶点属性的大小。在例子中，三维坐标是一个vec3，它由3个值组成，所以大小是3。
			第三个参数指定数据的类型，这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。
			第四个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。
			第五个参数叫做步长(Stride)，它告诉我们在连续的数据中，一个顶点的三维坐标到下一个顶点的三维坐标的间隔。由于下个顶点的三维坐标在3个float之后，我们把步长设置为3 * sizeof(float)。
			最后一个参数的类型是void*，所以需要我们进行这个奇怪的强制类型转换。它表示位置数据在所开辟的那片显存空间中起始位置的偏移量(Offset)。由于顶点的三维数据就在数组的开头，也就是那片显存空间中的开头，所以这里是 (void*) 0。
			*/
			glVertexAttribPointer(index, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.Offset);
			index++;
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}