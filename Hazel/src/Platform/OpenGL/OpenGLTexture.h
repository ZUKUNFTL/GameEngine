#pragma once

#include "Hazel/Renderer/Texture.h"
#include <glad/glad.h>

namespace Hazel {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void SetData(void* data, uint32_t size) override;

		// slot = 0，即默认设置当前纹理缓冲区到0号纹理单元上
		virtual void Bind(uint32_t slot = 0) const override;
		virtual bool operator==(const Texture& other) const override
		{
			/*
				1.*t2得到texture2d对象
				2.（OpenGLTexture2D&）*t2,将texture2d对象向下转换为OpenGLTexture2D对象
				若无&是代表转换到临时对象，临时对象不能被赋值
			*/

			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};

}