#include "hzpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel {
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		// Create an empty vertex shader handle
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		HZ_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}


	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		:m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::OpenGLShader(const std::string& pathfile)
	{
		std::string source = ReadFile(pathfile);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		// Extract name from filepath
		//assets/shaders/texture.glsl
		//找到最后一个斜杆的位置，这里windows可能会用\\来表示路径
		auto lastSlash = pathfile.find_last_of("/\\");
		//有可能没有/符号
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		//找到最后一个点的位置
		auto lastDot = pathfile.rfind('.');
		//有可能没有扩展名
		auto count = lastDot == std::string::npos ? pathfile.size() - lastSlash : lastDot - lastSlash;
		m_Name = pathfile.substr(lastSlash, count);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		UploadUniformInt(name, value);
	}
	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UploadUniformFloat(name, value);
	}
	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}
	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}
	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		/*
			第一个参数是 uniform 的位置值；
			第二个参数告诉OpenGL发送的矩阵个数，这里是1；
			第三个参数确定是否进行矩阵置换（交换行和列）。GLM中不需要，所以设置为 GL_FALSE;
			第四个参数是矩阵数据，但GLM并不是把矩阵存储为OpenGL接受的格式，因此这里用 glm::value_ptr 函数变换数据
		*/
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in) 
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			HZ_CORE_ERROR("Could not open file '{0}'", filepath);
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			//查找#type后面第一个回车位置
			size_t eol = source.find_first_of("\r\n", pos);
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			//这里默认认为#type后面有一个空格字符
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");
			//找到回车后面第一行第一个字符
			size_t nexLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nexLinePos);
			//如果nexLinePos是终止符，则最终的偏移量为字符串的末尾，如果不是则意味着后面还有shader
			shaderSources[ShaderTypeFromString(type)] = source.substr(nexLinePos, pos - (nexLinePos == std::string::npos ? source.size() - 1 : nexLinePos));
		}
		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		HZ_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
		//这里我们用array来代替vector来增加性能，栈中创建总是比堆中要块的
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;

		for (auto& kv : shaderSources) 
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;
			GLuint Shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(Shader, 1, &sourceCStr, 0);

			glCompileShader(Shader);

			GLint isCompiled = 0;
			glGetShaderiv(Shader, GL_COMPILE_STATUS, &isCompiled);
			//编译错误
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(Shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(Shader);

				HZ_CORE_ERROR("{0}", infoLog.data());
				HZ_CORE_ASSERT(false, "Vertex shader compilation failure!");
				break;
			}
			glAttachShader(program, Shader);
			glShaderIDs[glShaderIDIndex++]= Shader;
		}

		m_RendererID = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			
			for (auto id : glShaderIDs)
				glDeleteShader(id);

			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
			glDetachShader(program, id);
	}


}
