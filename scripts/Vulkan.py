#  =============================  3.Check Vulkan SDK environment  ======================================
import os
import subprocess
import sys
# Vulkan是在Setup.py相同路径下的Vulkan.py文件
from pathlib import Path

import Utils

from io import BytesIO
# urllib.requests是python提供打开和读取URL的库
from urllib.request import urlopen
from zipfile import ZipFile

# VULKAN_SDK代表系统环境变量里存的VULKAN_SDK的版本
VULKAN_SDK = os.environ.get('VULKAN_SDK')
# 官网上下载的url路径
VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.3.275.0/windows/VulkanSDK-1.3.275.0-Installer.exe'
HAZEL_VULKAN_VERSION = '1.3.275.0'
# 下载到本地的相对路径
VULKAN_SDK_EXE_PATH = 'Hazel/vendor/VulkanSDK/VulkanSDK.exe'

# 核心的安装SDK的函数
def InstallVulkanSDK():
    print('Downloading {} to {}'.format(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH))
    Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
    print("Done!")
    print("Running Vulkan SDK installer...")
    os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
    print("Re-run this script after installation")

# 当SDK未被安装, 或版本不对时, 调用此函数, 进而调用InstallVulkanSDK
def InstallVulkanPrompt():
    print("Would you like to install the Vulkan SDK?")
    install = Utils.YesOrNo()
    if (install):
        InstallVulkanSDK()
        quit()
        
# 检查Vulkan SDK是否被安装
def CheckVulkanSDK():
    if (VULKAN_SDK is None):
        print("You don't have the Vulkan SDK installed!")
        InstallVulkanPrompt()
        return False
    # 目前所用的sdk版本是1.2.170.0
    elif (HAZEL_VULKAN_VERSION not in VULKAN_SDK):
        print(f"Located Vulkan SDK at {VULKAN_SDK}")
        print(f"You don't have the correct Vulkan SDK version! (Hazel requires {HAZEL_VULKAN_VERSION})")
        # 调用此文件内的InstallVulkanPrompt函数, 可以在命令行安装Vulkan SDK
        InstallVulkanPrompt()
        return False
    
    print(f"Correct Vulkan SDK located at {VULKAN_SDK}")
    return True

VulkanSDKDebugLibsURL = 'https://sdk.lunarg.com/sdk/download/1.3.275.0/windows/VulkanSDK-1.3.275.0-DebugLibs.zip'
OutputDirectory = "Hazel/vendor/VulkanSDK"
TempZipFile = f"{OutputDirectory}/VulkanSDK.zip"

def CheckVulkanSDKDebugLibs():
    shadercdLib = Path(f"{OutputDirectory}/Lib/shaderc_sharedd.lib")
    if (not shadercdLib.exists()):
        print(f"No Vulkan SDK debug libs found. (Checked {shadercdLib})")
        print("Downloading", VulkanSDKDebugLibsURL)
        # with关键词是为了在urlopen失败时
        with urlopen(VulkanSDKDebugLibsURL) as zipresp:
            with ZipFile(BytesIO(zipresp.read())) as zfile:
                zfile.extractall(OutputDirectory)

    print(f"Vulkan SDK debug libs located at {OutputDirectory}")
    return True
