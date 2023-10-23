#pragma once


namespace Hazel {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual std::string GetName() const = 0;

		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& filepath);
	};

	//存储shader的库
	class ShaderLibrary
	{
	public:
		//用自定义名字添加shader
		void Add(const std::string& name, const Ref<Shader>& shader);
		//添加shader
		void Add(const Ref<Shader>& shader);
		//加载shader
		Ref<Shader> Load(const std::string& filepath);
		//用自定义名字加载shader
		Ref<Shader> Load(const std::string& name, const std::string& filepath);
		//通过名字获取shader
		Ref<Shader> Get(const std::string& name);

		//判断shader是否存在
		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}