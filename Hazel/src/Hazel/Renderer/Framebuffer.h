#pragma once

#include "Hazel/Core/Base.h"

//帧缓冲(个人理解)
//1. 可以将OpenGL渲染的场景放在这个帧缓冲中
//2. 然后可以把这个帧缓冲当做是颜色或者纹理采样区（取决于帧缓冲附加的缓冲附件类型）
//3. 在别处(Imgui)把这个帧缓冲当做颜色纹理渲染出来，就在ImGui界面上显示了原本应显示在屏幕上的场景

namespace Hazel {
	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		// FramebufferFormat Format = 
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};


	class Framebuffer
	{
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}

