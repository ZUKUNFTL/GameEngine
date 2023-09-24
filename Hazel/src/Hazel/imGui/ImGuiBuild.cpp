/*
游戏引擎，比如Unity、UE4里的窗口都是可以拖拽(Docking)的，这是编辑器最基本的功能，为了不采用WPF、QT这些技术来完成拖拽功能，可以直接用ImGui来完成，而ImGui在其Dock分支正在开发这一功能，还没合并到master上，这意味着这个相关功能可能会随时更新，还记得之前的ImGui的相关代码怎么做的吗，我是把相关代码抽离出来，放到了自己的ImGuiLayer.cpp里，然而这么做的缺点是，当ImGui改动代码的时候，我必须去看它改动了什么，然后再去手动Merge改动，这也不符合我使用git submodule的初衷，之前使用submodule的时候，都是引用该Project，然后把该submodule的源文件放进来，然而这里由于imgui里的源文件很多是我们不需要的，所以这里把其中的重要文件放到了ImGuiBuild.cpp里，直接当作头文件include进来，源码就不加在project的source列表里了，如下图所示，这里没有我们需要的cpp文件：
*/

#include "hzpch.h"

#define  IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "examples/imgui_impl_opengl3.cpp"
#include "examples/imgui_impl_glfw.cpp"