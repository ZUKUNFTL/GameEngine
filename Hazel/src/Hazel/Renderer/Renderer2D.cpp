#include "hzpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
namespace Hazel {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();

		// 1.创建顶点数组
		s_Data->QuadVertexArray = VertexArray::Create();
		//渲染正方形的纹理颜色。这里纹理坐标是基于左下角为中心算的，负的为0，正的为1，比如(-0.5f, 0.5f)的坐标纹理是(0.0f, 1.0f)，可以看结果图，更好理解
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		// 2.创建顶点缓冲区
		Ref<VertexBuffer> squareVB;
		squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));

		// 2.1设置顶点缓冲区布局
		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Positon"},
			{ShaderDataType::Float2, "a_TexCoord"}
		});

		// 1.1顶点数组添加顶点缓冲区，并且在这个缓冲区中设置布局
		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		// 3.索引缓冲
		Ref<IndexBuffer> squareIB;

		squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		// 1.2顶点数组设置索引缓冲区
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		//把贴图里的数据，即RGBA对应的四个字节，改为白色(0xffffffff)
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		//带有纹理的shader
		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->TextureShader->Bind();
		//将片段着色器的u_Texture取样的纹理批次设置为0号纹理单元
		s_Data->TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		// 手动管理内存
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->TextureShader->SetFloat4("u_Color", color);
		s_Data->WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) */*rotation*/ glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform);


		s_Data->QuadVertexArray->Bind();// 绑定顶点数组
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		// 绑定纹理//
		/*
		1.这个纹理的m_RendererID（纹理缓冲区的ID）是1
		2.texture->Bind();后，这个函数将m_RendererID=1的纹理缓冲区绑定到0号纹理单元上
		 (1)texture的实参是m_CheckerboardTexture
		 (2)m_CheckerboardTexture加载了Checkerboard.png图片
		 (3)所以m_RendererID=1的纹理缓冲区是Checkerboard.png的图片数据
		3.而之前代码s_Data->TextureShader->SetInt("u_Texture", 0);设置了片段着色器的u_Texture代表0号纹理单元
		4.所以片段采样器采样0号纹理单元的数据的时候，就是采样Checkerboard.png图片数据，所以Checkerboard.png图片会覆在Quad表面上
		*/
		s_Data->TextureShader->Bind();// 绑定shader
		//将纹理缓冲区绑定到0号纹理单元上
		texture->Bind(0);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) */*rotation*/ glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetMat4("u_Transform", transform);
		s_Data->TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));


		s_Data->QuadVertexArray->Bind();// 绑定顶点数组
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

}
