import shutil
import subprocess

label = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).strip()

print(label)

if __name__ == "__main__":
	directory = "latest-"+label
	configuration = "Release"

	try:
		shutil.rmtree(directory + "/projects")
	except shutil.Error as e:
		pass
	except OSError as e:
		pass

	shutil.copytree("projects", directory + "/projects", 
		ignore=shutil.ignore_patterns(
			"minigolf",
			"gravitycar", 
			"*.mpkg", 
			"imgui.ini")
		)

	shutil.copyfile("bin/" + configuration + "/tool.exe", directory + "/tool.exe")
	shutil.copyfile("bin/" + configuration + "/runtime.dll", directory + "/runtime.dll")
	shutil.copyfile("bin/" + configuration + "/DevIL.dll", directory + "/DevIL.dll")
	shutil.copyfile("bin/" + configuration + "/ILU.dll", directory + "/ILU.dll")
	shutil.copyfile("bin/" + configuration + "/ILUT.dll", directory + "/ILUT.dll")
	shutil.copyfile("bin/" + configuration + "/lua51.dll", directory + "/lua51.dll")
	shutil.copyfile("bin/" + configuration + "/SDL2.dll", directory + "/SDL2.dll")

	testlog = subprocess.check_output([directory + '/tool.exe', 'shooter', '-test'])

	if testlog.find("METACADE_EXIT_SUCCESS") != -1:
		shutil.make_archive("metacade-latest-"+label, "zip", ".", directory)
		shutil.rmtree(directory)
	else:
		print(testlog)