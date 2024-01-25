#pragma once
#include "Hazel/Core/Base.h"
#include "Layer.h"

#include <vector>

//层系统
namespace Hazel {

	class  LayerStack
	{

	public:
		LayerStack();
		~LayerStack();
		/*
			我们希望在进行游戏循环的时候，每个被启用的层（Layer）都会按照层栈顺序更新。
			通过这种更新循环，可以在层上进行渲染。显然因为层栈是有顺序的，这很重要。
			意味着你可以把层放在其他层的上面，这会决定绘制顺序。层栈对于构建、覆层系统也很有用。
			覆层能让你把层推至层栈的后半部分。
			注意我们搞了一个 overlay，这个 overlay 就是我们说的覆层。
			也就是说，推入一个 layer，就想正常地 push 进去，通过类中的迭代器m_LayerInsert放入，
			m_LayerInsert也就是正常 layer 的最后一个位置；而要是是 overlay ，我们就不需要管迭代器
			m_LayerInsert 的位置，而是直接 emplace_back 进去，因此 LayerStack 中的排列像是这样 ：
			layer layer overlayer overlayer ，而 m_LayerInsert 就应该指向这里第二个 layer 的位置，
			即最后一个非 overlayer 的 layer。
		*/

		//推入普通层
		void PushLayer(Layer* layer);
		//推入覆层
		void PushOverlay(Layer* overlay);
		//推出层
		void PopLayer(Layer* layer);
		//推出覆层
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};

}