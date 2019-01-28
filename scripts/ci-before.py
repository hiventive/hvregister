# CI: Before script

import os
import sys
import argparse
from distutils.dir_util import copy_tree
from os.path import dirname, abspath
from os.path import expanduser
import shutil
import platform
import yaml
from conan.packager import ConanMultiPackager

cppStd = 11
conanRemotes = [("https://conan.hiventive.com", True, "hiventive"),
				("https://api.bintray.com/conan/bincrafters/public-conan", True, "bincrafters")]
envVars = []

def printError(message, errorCode=None):
	sys.stderr.write(message + "\n")
	if errorCode:
		sys.stderr.write('Error code = ' + str(errorCode) + "\n")
	sys.exit(1)

def runCommand(command):
	returnCode = os.system(command)
	if returnCode != 0:
		sys.stderr.write("Command '" + command + "' failed\n")
		sys.stderr.write('Return code = ' + str(returnCode) + "\n")
		sys.exit(1)

def printDependenciesVersion():
	runCommand("python3 --version")
	if platform.system() == "Windows":
		if "VISUAL_STUDIO_TOOLSET" not in os.environ:
			printError("VISUAL_STUDIO_TOOLSET environment variable is not defined!")
	else:
		if "CC" not in os.environ:
			printError("CC environment variable is not defined!")
		runCommand("$CC --version")
		if "CXX" not in os.environ:
			printError("CXX environment variable is not defined!")
		runCommand("$CXX --version")
	runCommand("cmake --version")
	runCommand("conan --version")
	if platform.system() == "Windows":
		runCommand('powershell -Command "$env:CONAN_USER_HOME=\'' + getConanSourceUserHome() + '\'; conan profile show default"')
	else:
		runCommand("CONAN_USER_HOME=" + getConanSourceUserHome() + " conan profile show default")

def cleanConanTesting():
	conanUserHome = os.path.join(dirname(dirname(abspath(__file__))), ".conan")
	for root, dirs, files in os.walk(conanUserHome):
		level = root.replace(conanUserHome, '').count(os.sep)
		if level != 6:
			continue
		if os.path.basename(root) == "testing":
			packageDir = os.path.dirname(os.path.dirname(os.path.dirname(root)))
			shutil.rmtree(packageDir)
			print(packageDir + " removed")

def setupConan(local):
	conanSourceUserHome = getConanSourceUserHome()
	if local:
		projectWorkspace = dirname(abspath(__file__))
	else:
		projectWorkspace = dirname(dirname(abspath(__file__)))
	if os.path.isdir(conanSourceUserHome):
		copy_tree(conanSourceUserHome, os.path.join(projectWorkspace, ".conan"))
	else:
		printError("Unable to find " + conanSourceUserHome)
	with open(os.devnull, 'w') as devnull:
		cmp = ConanMultiPackager(out=devnull.write, remotes=conanRemotes)
		cmp.remotes_manager.add_remotes_to_conan()

def getConanSourceUserHome():
	homeDir = expanduser("~")
	if platform.system() == "Windows":
		conanUserHomeName = ".conan-" + str(os.environ["VISUAL_STUDIO_TOOLSET"])
	else:
		conanUserHomeName = ".conan-" + str(os.environ["CC"])
	return os.path.join(homeDir, conanUserHomeName)

def getConanUserHome(local):
	if local:
		projectWorkspace = dirname(abspath(__file__))
	else:
		projectWorkspace = dirname(dirname(abspath(__file__)))
	return os.path.join(projectWorkspace, ".conan")

def setupConanEnv(local):
	conanEnv = []
	conanEnv.append(["CONAN_USER_HOME", getConanUserHome(local)])
	conanName = next((item for item in envVars if item[0] == 'CONAN_NAME'), None)
	projectVersion = next((item for item in envVars if item[0] == 'PROJECT_VERSION'), None)
	if conanName and projectVersion:
		conanEnv.append(["CONAN_REFERENCE", conanName[1] + '/' + projectVersion[1]])
	if platform.system() == "Linux":
		conanEnv.append(["CONAN_ENV_CPPSTD", str(cppStd)])
		conanEnv.append(["CONAN_ENV_COMPILER_LIBCXX", "libstdc++11"])
	if platform.system() == "Windows" and "VISUAL_STUDIO_TOOLSET" in os.environ:
		conanEnv.append(["CONAN_ENV_COMPILER_TOOLSET", os.environ["VISUAL_STUDIO_TOOLSET"]])
	return conanEnv

def setupSettingsEnv():
	settingsEnv = []
	if os.path.exists("settings.yml"):
		with open("settings.yml", 'r') as settingesFile:
			try:
				settings = yaml.load(settingesFile)
				def toEnv(d, root=''):
					for k, v in d.items():
						if isinstance(v, dict):
							if root != '':
								toEnv(v, k + "_" + root)
							else:
								toEnv(v, k)
						else:
							settingsEnv.append([str(root + "_" + k).upper(), v])
				toEnv(settings)
			except:
				pass
	return settingsEnv

def setupEnv(local):
	envVars.extend(setupSettingsEnv())
	envVars.extend(setupConanEnv(local))
	for envVar in envVars:
		if platform.system() == "Windows":
			print('$Env:' + envVar[0] + ' = "' + envVar[1] + '"')
		else:
			print("export " + envVar[0] + "=" + envVar[1])

def init(local):
	printDependenciesVersion()
	setupConan(local)

parser = argparse.ArgumentParser(description="CI Before Script")
parser.add_argument('command', help='Command to run')
parser.add_argument('--local', action="store_true", dest="local", default=False, help='Run locally')

if __name__ == '__main__':
	args = parser.parse_args(sys.argv[1:])

	if args.command == 'setup-env':
		setupEnv(args.local)
	elif args.command == 'init':
		init(args.local)
	elif args.command == 'clean-conan-testing':
		cleanConanTesting()
	else:
		print('Unknown command')

