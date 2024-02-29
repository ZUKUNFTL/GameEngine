#include "hzpch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace Hazel{
	//工具类
	namespace Utils {
		char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);//以二进制打开文件，并将文件指针定位到文件的末尾
			if (!stream) {
				//打开文件失败
				return nullptr;
			}
			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);//将文件读取位置移动到文件的开头
			uint32_t size = end - stream.tellg();
			if (size == 0) {
				// 文件是空
				return nullptr;
			}
			char* buffer = new char[size];
			stream.read((char*)buffer, size); // 读入char字符数组中
			stream.close();

			*outSize = size; // 指针返回大小
			return buffer;	// 返回字符数组的首位置
		}

		MonoAssembly* LoadCSharpAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			// 除了加载程序集之外，我们不能将此图像image用于任何其他用途，因为此图像没有对程序集的引用
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			//如果打开图像成功（status == MONO_IMAGE_OK），则继续加载程序集
			if (status != MONO_IMAGE_OK) {
				const char* erroMessage = mono_image_strerror(status);
				// 可以打印错误信息
				return nullptr;
			}
			std::string pathString = assemblyPath.string();
			//这里调用了 mono_assembly_load_from_full 函数来加载程序集，其中 image 是之前打开的图像，assemblyPath.c_str() 是程序集的路径，status 是用于存储加载程序集状态的变量。
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			//关闭之前打开的图像，释放资源。
			mono_image_close(image);

			// 释放内存
			delete[] fileData;
			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			// 打印加载的c#程序的信息
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				HZ_CORE_TRACE("{}.{}", nameSpace, name);// 命名空间和类名
			}
		}
	}

	struct ScriptEngineData {
		MonoDomain* RootDomain = nullptr;//表示脚本引擎的根域
		MonoDomain* AppDomain = nullptr; //表示应用程序域

		MonoAssembly* CoreAssembly = nullptr;//表示核心程序集
		MonoImage* CoreAssemblyImage = nullptr;//表示核心程序集的图像
		ScriptClass EntityClass;
	};

	static ScriptEngineData* s_Data = nullptr;

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();
		//初始化mono
		InitMono();
		// 加载c#项目导出的dll
		LoadAssembly("Resources/Scripts/Hazel-ScriptCore.dll");

		// 添加内部调用//
		ScriptGlue::RegisterFunctions();

		s_Data->EntityClass = ScriptClass("Hazel", "Entity");

		//从核心程序集的图像中获取指定命名空间和类名的 Mono 类。
		// 4.创建一个Main类构成的mono对象并且初始化
		MonoObject* instance = s_Data->EntityClass.Instantiate();

		// 5.1调用main类的函数-无参
		MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethod("PrintMessage", 0);
		s_Data->EntityClass.InvokeMethod(instance, printMessageFunc);

		// 5.2调用main类的函数-带参
		MonoMethod* printIntFunc = s_Data->EntityClass.GetMethod("PrintInt", 1);

		int value = 5;
		void* param = &value;

		s_Data->EntityClass.InvokeMethod(instance, printIntFunc, &param);

		MonoMethod* printIntsFunc = s_Data->EntityClass.GetMethod("PrintInts", 2);

		int value2 = 505;
		void* params[2] = {
			&value,
			&value2
		};
		s_Data->EntityClass.InvokeMethod(instance, printIntsFunc, params);

		// 带string的函数
		MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from cpp!");
		MonoMethod* printCustomMessageFunc = s_Data->EntityClass.GetMethod("PrintCustomMessage", 1);
		void* stringParam = monoString;
		s_Data->EntityClass.InvokeMethod(instance, printCustomMessageFunc, &stringParam);
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
	}

	void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		// 创建一个app domain
		s_Data->AppDomain = mono_domain_create_appdomain("HazelScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// 加载c#项目导出的dll
		s_Data->CoreAssembly = Utils::LoadCSharpAssembly(filepath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		Utils::PrintAssemblyTypes(s_Data->CoreAssembly);// 打印dll的基本信息
	}

	void ScriptEngine::InitMono()
	{
		// 设置程序集装配路径(复制的4.5版本的路径),告诉 Mono 运行时在哪里查找程序集文件
		mono_set_assemblies_path("mono/lib");

		//初始化 Mono JIT 编译器，并创建一个根域
		MonoDomain* rootDomian = mono_jit_init("HazelJITRuntime");
		HZ_CORE_ASSERT(rootDomian);

		// 存储root domain指针
		s_Data->RootDomain = rootDomian;
	}

	void ScriptEngine::ShutdownMono()
	{
		// 对mono的卸载有点迷糊，以后再解决
		// mono_domain_unload(s_Data->AppDomain);
		//将存储在 ScriptEngineData 结构体中的应用程序域指针设为 nullptr，这表示当前应用程序域不再被使用
		s_Data->AppDomain = nullptr;

		// mono_jit_cleanup(s_Data->RootDomain);
		//将存储在 ScriptEngineData 结构体中的根域指针设为 nullptr，表示根域不再被使用
		s_Data->RootDomain = nullptr;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		// 1.创建一个Main类构成的mono对象并且初始化
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);// 构造函数在这里调用
		return instance;
	}

	//
	// ScriptClass
	//
	// C#类对应的Mono类
	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
	{
		//从核心程序集中获取指定命名空间和类名对应的 MonoClass 结构体，并将其存储在类的成员变量 m_MonoClass 中
		m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
	}
	MonoObject* ScriptClass::Instantiate()
	{
		//将脚本类的 MonoClass 对象传递给该函数进行实例化。
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}
	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		//从脚本类的 MonoClass 对象中获取指定名称和参数数量的方法。
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}
	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		//传递要调用的方法、实例对象以及参数列表，执行指定方法，并返回执行结果。
		return mono_runtime_invoke(method, instance, params, nullptr);// **类型，&params(实参) = params（实参）
	}
}