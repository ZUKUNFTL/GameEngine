#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include "Hazel/Scene/Components.h"
namespace Hazel {


	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{

		//遍历所有实体
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.each([&](auto entityID) {
		// ImGui显示场景的实体
		Entity entity{ entityID, m_Context.get() };
		DrawEntityNode(entity);
		});
		ImGui::End();
	}

	//将需要的信息写出来
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		// 若是被点击标记为选中状态|有下一级
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		// 第一个参数是唯一ID 64的，
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectionContext = entity;
		}

		//示例(模拟被点击展开的事件)
		if (opened) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)98476565, flags, tag.c_str());
			if (opened) {
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

	}

}