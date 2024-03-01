#include "hzpch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace Hazel{
	//工具类
	namespace Utils {
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
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

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
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

		static void PrintAssemblyTypes(MonoAssembly* assembly)
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

		//因为加载了两个dll，因此需要两个程序集
		MonoAssembly* CoreAssembly = nullptr;//表示核心程序集
		MonoImage* CoreAssemblyImage = nullptr;//表示核心程序集的图像

		MonoAssembly* AppAssembly = nullptr;  //表示脚本程序集
		MonoImage* AppAssemblyImage = nullptr;//表示脚本程序集的图像

		ScriptClass EntityClass;// 存储C#父类Entity的Mono类
		// 所有C#脚本map (脚本map)，存储封装的Mono类
		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		// 需要运行的C#脚本map（运行脚本map），存储封装的Mono类对象
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;
		// 运行时的场景
		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();
		//初始化mono
		InitMono();
		// 加载c#项目导出的dll
		LoadAssembly("Resources/Scripts/Hazel-ScriptCore.dll");
		//找到dll里所有继承Entity的类，表明这是脚本类
		LoadAppAssembly("SandboxProject/Assets/Scripts/Binaries/Sandbox.dll");
		LoadAssemblyClasses();

		//注册脚本组件
		ScriptGlue::RegisterComponents();
		// 添加内部调用
		ScriptGlue::RegisterFunctions();

		s_Data->EntityClass = ScriptClass("Hazel", "Entity", true);
#if 0

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

		HZ_CORE_ASSERT(false);
#endif
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
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		Utils::PrintAssemblyTypes(s_Data->CoreAssembly);// 打印dll的基本信息
	}

	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		// Move this maybe
		s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath);
		auto assemb = s_Data->AppAssembly;
		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
		auto assembi = s_Data->AppAssemblyImage;
		// Utils::PrintAssemblyTypes(s_Data->AppAssembly);
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;

		s_Data->EntityInstances.clear();
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();		// 得到这个实体的组件
		if (ScriptEngine::EntityClassExists(sc.ClassName)) {			// 组件的脚本名称是否正确
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);// 实例化类对象，并存储OnCreate、OnUpdate函数，调用父类Entity的构造函数，传入实体的UUID
			s_Data->EntityInstances[entity.GetUUID()] = instance;	// 运行脚本map存储这些ScriptInstance(类对象)
			instance->InvokeOnCreate();								// 调用C#的OnCreate函数
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts)
	{
		UUID entityUUID = entity.GetUUID();
		HZ_CORE_ASSERT(s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end());

		Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
		instance->InvokeOnUpdate((float)ts);
	}

	Hazel::Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	std::unordered_map<std::string, Hazel::Ref<Hazel::ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
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
		// 创建一个Main类构成的mono对象并且初始化
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);// 构造函数在这里调用
		return instance;
	}

	//找到dll里所有继承Entity的类，表明这是脚本类，得到对应的封装的Mono类（119封装的）
	//并用脚本map存储所有封装的mono类（用封装的Mono类实例化）
	void ScriptEngine::LoadAssemblyClasses()
	{
		//先清空脚本类map，以便重新加载新的类
		s_Data->EntityClasses.clear();
		//获取程序集中的类型定义表信息，这个表中包含了程序集中所有的类型信息。
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		//获取类型定义表中的行数，即程序集中定义的类型的数量
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		//获取表示 "Hazel.Entity" 类的 MonoClass 对象，即父类对象。
		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Hazel", "Entity");

		// 遍历类型定义表中的每一行
		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);
			// 从游戏逻辑脚本库里加载脚本类
			//获取类型的命名空间和名称。
			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0) {
				fullName = fmt::format("{}.{}", nameSpace, name);
			}
			else {
				fullName = name;
			}
			// 加载Dll中所有C#类
			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, name);
			if (monoClass == entityClass) {// entity父类不保存
				continue;
			}
			// 判断当前类是否为Entity的子类
			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false); // 这个c#类是否为entity的子类
			if (isEntity) {
				// 存入封装的Mono类对象
				// 并存入脚本map中
				s_Data->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
			}
		}
	}

	//
	// ScriptClass
	//
	// C#类对应的Mono类
	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		/*
			得到C# dll的Mono类
			应该**区分**是获取C#核心库程序集的 Mono类
			还是获取C#应用程序集的 Mono类
		*/
		//从程序集中获取指定命名空间和类名对应的 MonoClass 结构体，并将其存储在类的成员变量 m_MonoClass 中
		m_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());
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

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass) 
	{
		// 获取Sandbox Player类构成的MonoObject对象，相当于new Sandbox.Player()
		m_Instance = scriptClass->Instantiate();
		// 这里不一样，是获取父类Entity的构造函数
		m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);// 获取并存储Sandbox.Player类的函数
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		// 调用C#Entity类的构造函数
		{
			UUID entityID = entity.GetUUID();
			void* param = &entityID;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);// 第一个参数传入的是Entity子类(Player)构成的mono对象
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_OnUpdateMethod)
		{
			void* param = &ts;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
		}
	}

}