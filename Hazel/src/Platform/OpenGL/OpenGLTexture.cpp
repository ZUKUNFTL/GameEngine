#include "hzpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Hazel {


	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path)
	{
		int width, height, channels;
		// stbi_set_flip_vertically_on_load(1);// 设置垂直翻转，由于OpenGL是从上往下读取它并设置，而TB_IMAGE成像默认时从上到下读取它，所以要设置
		stbi_set_flip_vertically_on_load(1);
		/*
			路径，加载完返回大小，通道rgb、rbga，返回的字符串指针指向的就是读取的纹理图片数据！
		*/
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		HZ_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4) 
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		/*创建1个纹理，生成纹理缓冲区返回id给变量*/ // 是GL_TEXTURE_2D，写错过GL_TEXTURE
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		/*告诉OpenGLm_RendererID的纹理存储的是rbg8位，宽高的缓冲区*/
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		/*告诉opengl，纹理缩小时用线性过滤*/
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		/*告诉opengl，纹理放大时用周围颜色的平均值过滤*/
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		/*指定截取子区域，将纹理图片数据给上传OpenGL。*/
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
		/*数据上传GPU后，CPU的数据需要释放*/
		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot /*= 0*/) const
	{
		//因为你可以绑定很多的纹理，因此SLOT表示对应纹理的槽位
		glBindTextureUnit(slot, m_RendererID);
	}

}