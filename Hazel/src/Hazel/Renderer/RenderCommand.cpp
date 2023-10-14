#include "hzpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
namespace Hazel {

	//这里我们并不需要担心内存泄漏的问题，因为它会持续整个应用的整个声明周期
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}