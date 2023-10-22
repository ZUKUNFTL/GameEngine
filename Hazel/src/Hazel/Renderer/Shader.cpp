#include "hzpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			//#if HZ_PLATFORM_WINDOWS
		case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
			//#endif
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Hazel::Shader* Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			//#if HZ_PLATFORM_WINDOWS
		case RendererAPI::API::OpenGL: return new OpenGLShader(filepath);
			//#endif
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}