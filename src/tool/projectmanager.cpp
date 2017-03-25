#include "native.h"
#include "metacade_amal.h"

using namespace Arcade;

#include "projectmanager.h"

#include <iostream>
#include <windows.h>

CProjectManager::CProjectManager(shared_ptr<NativeEnv> native, const CString& projectDirectory)
	: _native(native)
	, _projectDirectory(projectDirectory)
{

}

bool CProjectManager::enumerateProjectFolders(std::vector<CProject>& projects)
{
	CString dir = _projectDirectory / "*";

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(*dir, &f);
	if (h == INVALID_HANDLE_VALUE) return false;
	do
	{
		if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CString folder = CString(f.cFileName);
			CString projectPath = _projectDirectory / folder;
			if ( folder == "." || folder == ".." ) continue;

			enumerateProjectFiles(projectPath, projects);
		}
	} while (FindNextFile(h, &f));

	return true;
}

bool CProjectManager::enumerateProjectFiles(const CString& folder, vector<CProject>& projects)
{
	CString dir = folder / "*";

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(*dir, &f);
	if (h == INVALID_HANDLE_VALUE) return false;
	do
	{
		if (!(f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			CString filename = CString(f.cFileName);
			CString projectFilePath = _projectDirectory / filename;

			if ( filename.endsWith(".mproject") )
			{
				std::cout << "PROJECT FILE: " << *projectFilePath << std::endl;
			}

			IFileObject* file = _native->getFileSystem()->openFile(projectFilePath, FILE_READ);
			if ( file != nullptr )
			{
				CProject project;
				if ( project.load(file) )
				{
					projects.push_back(project);
				}

				_native->getFileSystem()->closeFile(file);
			}
		}
	} while (FindNextFile(h, &f));

	return true;
}
