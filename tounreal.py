import shutil
import amalgam

destination = "G:/Unreal/Projects/ThingsAndStuff/Plugins/Arcade"

def copyDirectory(src, dest):
	try:
		shutil.copytree(src, dest)
	except shutil.Error as e:
		print('Directory not copied. Error: %s' % e)
		return False
	except OSError as e:
		print('Directory not copied. Error: %s' % e)
		return False

	return True

if __name__ == "__main__":
	print("Amalgamating Headers...")
	amalgam.amalgamate()

	print("Copying build files to unreal...")
	shutil.copyfile("bin/Release/runtime.dll", destination + "/Binaries/Win64/runtime.dll")
	shutil.copyfile("lib/Release/runtime.lib", destination + "/ThirdParty/Arcade/Libraries/ArcadeRuntime.x64.lib")
	shutil.copyfile("src/runtime/metacade_amal.h", destination + "/ThirdParty/Arcade/Includes/metacade_amal.h")

	print("Done")
	exit(0)