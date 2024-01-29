#  =============================  1.Check Python environment  ======================================
# The pkg_resources module provides runtime facilities for finding, introspecting, activating and using installed Python distributions.
import subprocess
import pkg_resources

def install(package):
    print(f"Installing {package} module...")
    subprocess.check_call(['python', '-m', 'pip', 'install', package])

def ValidatePackage(package):
    required = { package }
    installed = {pkg.key for pkg in pkg_resources.working_set}
    missing = required - installed
	# 安装缺失的package
    if missing:
        install(package)

def ValidatePackages():
    ValidatePackage('requests')
    ValidatePackage('fake-useragent')
# 这里是保证python环境是完整的