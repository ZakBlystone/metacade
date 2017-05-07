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
			CString projectFilePath = folder / filename;

			if ( filename.endsWith(".mproject") )
			{
				std::cout << "PROJECT FILE: " << *projectFilePath << std::endl;
			}
			else
			{
				continue;
			}

			IFileObject* file = _native->getFileSystem()->openFile(projectFilePath, FILE_READ);
			if ( file != nullptr )
			{
				CProject project(projectFilePath, folder);
				if ( project.load(file) )
				{
					std::cout << " -loaded" << std::endl;
					projects.push_back(project);
				}
				else
				{
					std::cout << " -failed to load" << std::endl;
				}

				_native->getFileSystem()->closeFile(file);
			}
			else
			{
				std::cout << " -failed to open" << std::endl;
			}
		}
	} while (FindNextFile(h, &f));

	return true;
}

bool CProjectManager::saveProject(const CProject& project)
{
	IFileObject* file = _native->getFileSystem()->openFile(project.getProjectPath(), FILE_WRITE);
	bool ok = project.save(file);

	_native->getFileSystem()->closeFile(file);
	return ok;
}

bool CProjectManager::saveProjectAs(const CProject& project, const CString& newFile)
{
	IFileObject* file = _native->getFileSystem()->openFile(_projectDirectory / newFile, FILE_WRITE);
	bool ok = project.save(file);

	_native->getFileSystem()->closeFile(file);
	return ok;
}
