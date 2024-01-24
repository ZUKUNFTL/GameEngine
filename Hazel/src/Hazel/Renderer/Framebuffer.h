#pragma once

#include "Hazel/Core/Base.h"

//帧缓冲(个人理解)
//1. 可以将OpenGL渲染的场景放在这个帧缓冲中
//2. 然后可以把这个帧缓冲当做是颜色或者纹理采样区（取决于帧缓冲附加的缓冲附件类型）
//3. 在别处(Imgui)把这个帧缓冲当做颜色纹理渲染出来，就在ImGui界面上显示了原本应显示在屏幕上的场景

namespace Hazel {
	//纹理类别
	enum class FramebufferTextureFormat
	{
		None = 0,

		//Color颜色纹理
		RGBA8,
		RED_INTEGER,

		//Depth/stencil深度纹理
		DEPTH24STENCIL8,

		//Defaults
		Depth = DEPTH24STENCIL8
	};

	//具体纹理类
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			:TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	//需要附加的纹理集
	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			:Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};


	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		//读取鼠标位置像素的**帧缓冲中缓冲区（渲染目标）**的数据
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}

