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

	//�洢shader�Ŀ�
	class ShaderLibrary
	{
	public:
		//���Զ����������shader
		void Add(const std::string& name, const Ref<Shader>& shader);
		//���shader
		void Add(const Ref<Shader>& shader);
		//����shader
		Ref<Shader> Load(const std::string& filepath);
		//���Զ������ּ���shader
		Ref<Shader> Load(const std::string& name, const std::string& filepath);
		//ͨ�����ֻ�ȡshader
		Ref<Shader> Get(const std::string& name);

		//�ж�shader�Ƿ����
		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}