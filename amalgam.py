import re
import os

def block_comment_remover(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " "
        else:
            return s
    pattern = re.compile(
        r'/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)

def parseIncludeDirective(line):
	start = line.find("#include")
	if start != -1:
		filename = ".".join(re.findall(r'\"(.+?)\"', line))
		return filename
	return None


def loadInclude(filename):
	if not os.path.exists(filename):
		return ""

	path = filename[:filename.rfind('/')]

	code = ""
	with open(filename) as f:
		content = f.readlines()

		for line in content:
			lospace = re.sub("[\s\t\r\n]+", "", line)
			if line.find("#pragma once") != -1 or lospace == "":
				continue

			include = parseIncludeDirective(line)
			if include != None:
				code += loadInclude(path + "/" + include)
			else:
				code += line

	return "//" + filename + "\n" + block_comment_remover(code).strip() + "\n"


amal = loadInclude("src/runtime/metacade_public.h")
print ( amal )

headFile = open("HEAD", "r")
head = headFile.read().replace("<FILENAME>", "metacade_amal.h").replace("<DESCRIPTION>", "amalgamated header for metacade")

with open("src/runtime/metacade_amal.h", "w") as f:
	f.write(head + "\n")
	f.write("#pragma once\n")
	f.write(amal)