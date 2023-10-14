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

			//glEnableVertexAttribArray(0)�����˶�����ɫ���ģ�location = 0�������Ա���
			glEnableVertexAttribArray(index);

			/*
			@brief
			ʹ��glVertexAttribPointer�������Ը���OpenGL����ν�����Щ���ݣ�Ӧ�õ�������������ϣ������洢��VAO�У������ǻ���һ��������VBOʱ�����ǲ���Ҫ�ظ���������ֻ��Ҫ������Ҫ��VAO���С�ͬʱ����ֱ�Ӱ����ǵ�shader
			��һ������ָ������Ҫ���õĶ������ԡ����ڶ�����ɫ����ʹ��layout(location = 0)������position�������Ե�λ��ֵ(Location)���Ѷ������Ե�λ��ֵ����Ϊ0����Ϊ��������ϣ�������ݴ��ݵ���һ�����������У������������Ǵ���0��
			�ڶ�������ָ���������ԵĴ�С���������У���ά������һ��vec3������3��ֵ��ɣ����Դ�С��3��
			����������ָ�����ݵ����ͣ�������GL_FLOAT(GLSL��vec*�����ɸ�����ֵ��ɵ�)��
			���ĸ��������������Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣
			�����������������(Stride)�������������������������У�һ���������ά���굽��һ���������ά����ļ���������¸��������ά������3��float֮�����ǰѲ�������Ϊ3 * sizeof(float)��
			���һ��������������void*��������Ҫ���ǽ��������ֵ�ǿ������ת��������ʾλ�������������ٵ���Ƭ�Դ�ռ�����ʼλ�õ�ƫ����(Offset)�����ڶ������ά���ݾ�������Ŀ�ͷ��Ҳ������Ƭ�Դ�ռ��еĿ�ͷ������������ (void*) 0��
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