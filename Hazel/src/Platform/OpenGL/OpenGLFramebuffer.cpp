#include "hzpch.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

#include "glad/glad.h"
namespace Hazel {

	static const uint32_t s_MaxFramebufferSize = 8192;

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		:m_Specification(spec)
	{
		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			//每次生成缓冲帧先清除之前的一帧
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}

		// 1.创建帧缓冲
		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID); // 绑定这个帧缓冲

		// 2.创建纹理
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);;
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		//旧API
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 1.1纹理附加到帧缓冲
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		// 3.创建深度模板缓冲纹理附加到帧缓冲中
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);;
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		//新API
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		// 1.2深度模板缓冲纹理附加到帧缓冲中
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);// 取消绑定这个帧缓冲,以免不小心渲染到错误的帧缓冲上，比如深度、模板缓冲不会渲染到这里
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		// 在下次帧缓冲被绑定时，OpenGL视口大小被设置为imgui视口大小
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		//todo:
		//当窗口最小化，imgui的viewportsize会是负值传入给帧缓冲重新设置大小
		//但是帧缓冲的大小参数是无符号，所以会转换为一个很大的无符号整数
		//帧缓冲的大小就会过大，导致重新最大化窗口时，摄像机投影会变形（摄像机的宽高比与帧缓冲的大小不匹配）
		//判断帧缓冲大小如果过大就不重置，这里仍然没有解决这个问题，摄像机投影会变形问题（因为窗口最小化事件与窗口重新调整大小不是同一个事件）
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize) {
			HZ_CORE_ERROR("Attempt to resize frambuffer to {0} {1}", width, height);
			return;
		}
		//设置缓冲帧的大小
		m_Specification.Width = width;
		m_Specification.Height = height;
		//需要重新生成帧缓冲来调整
		Invalidate();
	}

}