# os代表操作系统, 为了让脚本能与os打交道, 这里相当于创建了一个OS模块的对象os
import os
# 创建subprocess模块对象, 可以调用其他的exe
import subprocess
import CheckPython

# Make sure everything we need is installed
CheckPython.ValidatePackages()

import Vulkan

# Change from Scripts directory to root
os.chdir('../')

# 调用CheckVulkanSDK函数
if (not Vulkan.CheckVulkanSDK()):
    print("Vulkan SDK not installed.")
    
if (not Vulkan.CheckVulkanSDKDebugLibs()):
    print("Vulkan SDK debug libs not found.")
    
#  =============================  4.Call premake5.exe to build solution  ======================================
print("Running premake...")
subprocess.call(["vendor/premake/bin/premake5.exe", "vs2019"])
