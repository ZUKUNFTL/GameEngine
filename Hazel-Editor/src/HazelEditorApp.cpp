#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "EditorLayer.h"
namespace Hazel {
	class HazelEditor :public Application
	{
	public:
		HazelEditor()
			:Application("Hazek Editor")
		{
			PushLayer(new EditorLayout());
		}

		~HazelEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new HazelEditor();
	}

}
