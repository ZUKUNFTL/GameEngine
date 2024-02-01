#include "hzpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"


namespace Hazel {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		// 注意格式是GL_RGBA8不是GL_RGBA, RGBA8是InternalFormat
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

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
		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		/*创建1个纹理，生成纹理缓冲区返回id给变量*/ // 是GL_TEXTURE_2D，写错过GL_TEXTURE
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		/*告诉OpenGLm_RendererID的纹理存储的是rbg8位，宽高的缓冲区*/
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		/*告诉opengl，纹理缩小时用线性过滤*/
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		/*告诉opengl，纹理放大时用周围颜色的平均值过滤*/
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//设置重复采样纹理（这是默认的）：GL_REPEAT,设置取邻近边缘像素:GL_CLAMP_TO_EDGE
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		/*指定截取子区域，将纹理图片数据给上传OpenGL。*/
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
		/*
			glTextureSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
			target参数必须是glCopyTexImage2D中的对应的target可使用的值。
			level 是mipmap等级。
			xoffset, yoffset是要修改的图像的左下角偏移。width，height是要修改的图像宽高像素。修改的范围在原图之外并不受到影响。
			format，type描述了图像数据的格式和类型，和glTexImage2D中的format, type一致。
			pixels 是子图像的纹理数据，替换为的值。
		*/
		/*数据上传GPU后，CPU的数据需要释放*/
		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		// 默认是用RGBA的格式
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		// 可以通过`glTextureSubImage2D`这个API，为Texture手动提供数据，创建这个WhiteTexture
		// 注意这里的格式是GL_RGBA, 这是贴图的DataFormat
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot /*= 0*/) const
	{
		//因为你可以绑定很多的纹理，因此SLOT表示对应纹理的槽位
		// 0号纹理单元默认开启，不需要写glActiveTexture(GL_TEXTURE0); 代码
		glBindTextureUnit(slot, m_RendererID);
	}

} 