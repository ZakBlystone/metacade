#pragma once

class CProject
{
public:
	//JSON HERE

private:
	friend class CProjectManager;

	bool load(IFileObject* file);
	bool save(IFileObject* file);
};