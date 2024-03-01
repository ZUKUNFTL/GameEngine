#pragma once

#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

// 如果不引入头文件，必须外部声明，但这些都是在c文件定义的结构，所以需要extern"C"
extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}
namespace Hazel {

	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};
	// 属性名称对应的结构体
	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;

		MonoClassField* ClassField;
	};

	//封装 加载C#类成Mono类 的类
	class ScriptClass {
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);// 3. 创建一个MonoClass类

		MonoObject* Instantiate();// 4.创建一个由MonoClass类构成的mono对象并且初始化
		MonoMethod* GetMethod(const std::string& name, int parameterCount);// 5.1 获取类的函数
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);// 5.2 调用类的函数

		//属性
		const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }
	private:
		std::map<std::string, ScriptField> m_Fields;

		std::string m_ClassNamespace;
		std::string m_ClassName;
		MonoClass* m_MonoClass = nullptr;
		friend class ScriptEngine;
	};

	//封装 由Mono类实例化的Mono类对象
	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);
		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);
		Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, const T& value)
		{
			SetFieldValueInternal(name, &value);
		}
	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);
	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		inline static char s_FieldValueBuffer[8];
	};

	//封装 加载构建Mono环境类
	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filepath);	// 加载dll程序集
		static void LoadAppAssembly(const std::filesystem::path& filepath);// 加载核心dll程序集

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static bool EntityClassExists(const std::string& fullClassName);
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, Timestep ts);

		static Scene* GetSceneContext();
		static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);
		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();

		static MonoImage* GetCoreAssemblyImage();

	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);	// 实例化Mono类为Mono实例对象

		static void LoadAssemblyClasses();

		friend class ScriptClass;
		friend class ScriptGlue;
	};

}
