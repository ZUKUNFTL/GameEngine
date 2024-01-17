#pragma once
#include "Scene.h"

namespace Hazel {
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		//序列化
		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);

		//反序列化
		bool Deserialize(const std::string& filepath);
		bool DeSerializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};

}
