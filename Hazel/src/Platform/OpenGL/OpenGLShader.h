#pragma once

#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>

typedef unsigned int GLenum;
namespace Hazel {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		OpenGLShader(const std::string& pathfile);
		~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetFloat(const std::string& name, float value) override;
		void SetFloat2(const std::string& name, const glm::vec2& value);
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& GetName() const override { return m_Name; };

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

		//将vulkan的glsl编译成SPIR - V二进制文件，并且保存在本地作为缓存
		void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
		//将Vulkan的glsl的SPIR-V二进制文件转换为OpenGL的glsl源文件字符串
		//再将OpenGL的glsl源文件字符串转换为SPIR-V二进制文件，并且保存在本地作为缓存
		void CompileOrGetOpenGLBinaries();
		//用OpenGL的API编译链接OpenGL版本的glsl的SPIR-V二进制文件(与原始编译链接OpenGL的GLSL着色器代码不太一样)
		void CreateProgram();

		//打印shader中的数据
		void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);
	private:
		uint32_t m_RendererID;
		std::string m_FilePath; //shader目录
		std::string m_Name;

		//vulkan的shader编译后的SPIR-V二进制数据
		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
		//Opengl的shader编译后的SPIR-V二进制数据
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;

		//Opengl的shader字符串数据
		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;
	};
}