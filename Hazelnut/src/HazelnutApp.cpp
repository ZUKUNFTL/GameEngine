#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "EditorLayer.h"
namespace Hazel {
	class Hazelnut :public Application
	{
	public:
		Hazelnut(ApplicationCommandLineArgs args)
			:Application("Hazelnut", args)
		{
			PushLayer(new EditorLayout());
		}

		~Hazelnut()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new Hazelnut(args);
	}

}
