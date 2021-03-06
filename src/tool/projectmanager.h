#include "project.h"

#pragma once

#include <vector>
#include <memory>

using std::shared_ptr;
using std::vector;

class CProjectManager
{
public:
	CProjectManager(shared_ptr<NativeEnv> native, const CString& projectDirectory);

	bool enumerateProjectFolders(vector<CProject>& projects);
	bool enumerateProjectFiles(const CString& folder, vector<CProject>& projects);

	bool saveProject(const CProject& project);
	bool saveProjectAs(const CProject& project, const CString& newFile);

private:
	CString _projectDirectory;
	shared_ptr<NativeEnv> _native;
};