#include "json.hpp"
#include "metacade_amal.h"

using namespace Arcade;
using nlohmann::json;

#include "project.h"
#include "compiler.h"
#include <memory>

using std::shared_ptr;
using std::make_shared;

CProject::CProject(const CString& filepath, const CString& rootpath)
	: _dirty(false)
	, _filepath(filepath)
	, _rootpath(rootpath)
{
	_assets.insert(make_pair(ASSET_SOUND, std::vector<CAssetFileDef>()));
	_assets.insert(make_pair(ASSET_TEXTURE, std::vector<CAssetFileDef>()));
}

CString CProject::getProjectName() const
{
	CString out;
	if ( getMetaValue("name", out) )
		return out;

	return getProjectPath();
}

CString CProject::getProjectPath() const
{
	return _filepath;
}

CString CProject::getRootDirectory() const
{
	return _rootpath;
}

bool CProject::getMetaValue(const CString &key, CString& value) const
{
	auto val = _meta.find(key);
	if ( val == _meta.end() ) return false;

	value = (*val).second;
	return true;
}

void CProject::setMetaValue(const CString &key, const CString& value)
{
	std::cout << " -> " << *key << " = " << *value << std::endl;
	_meta.insert(std::make_pair(key, value));
	makeDirty();
}

bool CProject::addAssetDef(EAssetType type, const CString& name, const CString& path)
{
	CAssetFileDef def;
	def._assetType = type;
	def._path = path;
	def._name = name;

	for ( auto asset : _assets[type] )
	{
		if ( asset._path == path ) return false;
		if ( asset._name == name && asset._assetType == type )
		{
			asset._path = path;
			makeDirty();
			return true;
		}
	}

	_assets[type].push_back(def);
	makeDirty();

	return true;
}

int32 CProject::numAssetDefs(EAssetType type) const
{
	auto vec = (*_assets.find(type)).second;
	return (int32) vec.size();
}

CAssetFileDef CProject::getAssetDef(EAssetType type, int32 num) const
{
	auto vec = (*_assets.find(type)).second;
	return vec[num];
}

void CProject::removeAssetDef(EAssetType type, int32 num)
{
	if ( num < 0 || num >= _assets[type].size() ) return;
	_assets[type].erase( _assets[type].begin() + num );
	makeDirty();
}

void CProject::makeDirty()
{
	_dirty = true;
}

bool CProject::isDirty() const
{
	return _dirty;
}

IPackage* CProject::buildPackage(IRuntime* runtime)
{
	IPackageManager* manager = runtime->getPackageManager();
	CPackageBuilder* builder = manager->createPackageBuilder(getProjectName());
	shared_ptr<CCompiler> compiler = make_shared<CCompiler>();
	
	builder->load();

	for ( auto entry : _meta )
	{
		builder->getMetaData()->setKeyValuePair( entry.first, entry.second );
	}

	for ( int32 i=0; i<numAssetDefs(ASSET_TEXTURE); ++i )
	{
		CAssetFileDef def = getAssetDef(ASSET_TEXTURE, i);
		CImageAsset* asset = builder->addNamedAsset<CImageAsset>(def._name);
		
		IMetaData* meta = runtime->createMetaData();
		std::cout << "COMPILE: " << *(_rootpath / def._path) << std::endl;
		meta->setKeyValuePair("file", _rootpath / def._path);

		bool success = compiler->compile(asset, meta);
		if ( !success )
		{
			std::cout << "ERROR: Failed to compile asset: " << *def._path << std::endl;
			builder->removeAsset(asset);
		}

		runtime->deleteMetaData(meta);
	}

	for ( int32 i=0; i<numAssetDefs(ASSET_SOUND); ++i )
	{
		CAssetFileDef def = getAssetDef(ASSET_SOUND, i);
		CSoundAsset* asset = builder->addNamedAsset<CSoundAsset>(def._name);
		
		IMetaData* meta = runtime->createMetaData();
		std::cout << "COMPILE: " << *(_rootpath / def._path) << std::endl;
		meta->setKeyValuePair("file", _rootpath / def._path);

		bool success = compiler->compile(asset, meta);
		if ( !success )
		{
			std::cout << "ERROR: Failed to compile asset: " << *def._path << std::endl;
			builder->removeAsset(asset);
		}

		runtime->deleteMetaData(meta);
	}

	if ( !builder->save() )
		return nullptr;

	manager->deletePackageBuilder(builder);

	if ( manager->findAndPreloadPackages() )
	{
		return manager->getPackageByName( getProjectName() );
	}

	return nullptr;
}

bool CProject::load(IFileObject* file)
{
	if ( file == nullptr ) return false;

	uint32 size = file->getSize();
	std::shared_ptr<uint8> mem = std::shared_ptr<uint8>(new uint8[size], [](uint8* ptr){ delete [] ptr; } );

	if ( !file->read(mem.get(), size) ) return false;

	try
	{
		auto data = json::parse( std::string((const char*) mem.get(), size) );
		if ( !data.is_object() ) return false;

		auto meta = data["meta"];
		auto assets = data["assets"];

		{
			for ( json::iterator it = meta.begin(); it != meta.end(); ++it )
			{
				std::string k = it.key();
				std::string v = it.value();
				setMetaValue( k.c_str(), v.c_str() );
			}
		}

		{
			auto textures = assets["textures"];
			for ( json::iterator it = textures.begin(); it != textures.end(); ++it )
			{
				std::string k = it.key();
				std::string v = it.value();
				addAssetDef(ASSET_TEXTURE, k.c_str(), v.c_str());
			}
		}

		{
			auto sounds = assets["sounds"];
			for ( json::iterator it = sounds.begin(); it != sounds.end(); ++it )
			{
				std::string k = it.key();
				std::string v = it.value();
				addAssetDef(ASSET_SOUND, k.c_str(), v.c_str());
			}
		}
	}
	catch ( std::exception e )
	{
		std::cout << e.what() << std::endl;
		return false;
	}

	_dirty = false;
	return true;
}

bool CProject::save(IFileObject* file) const
{
	if ( file == nullptr ) return false;

	try
	{
		json out = json::object();

		out["meta"] = json::object();


		out["assets"] = json::object();
		out["assets"]["textures"] = json::object();
		out["assets"]["sounds"] = json::object();

		for ( int32 i=0; i<numAssetDefs(ASSET_SOUND); ++i )
		{
			CAssetFileDef def = getAssetDef(ASSET_SOUND, i);
			out["assets"]["sounds"][ std::string(*def._name) ] = *def._path;
		}

		for ( int32 i=0; i<numAssetDefs(ASSET_TEXTURE); ++i )
		{
			CAssetFileDef def = getAssetDef(ASSET_TEXTURE, i);
			out["assets"]["textures"][ std::string(*def._name) ] = *def._path;
		}

		for ( auto it = _meta.begin(); it != _meta.end(); ++it )
		{
			out["meta"][ std::string(*(*it).first) ] = *(*it).second;
		}

		std::string outstr = out.dump(4);

		return file->write(outstr.c_str(), outstr.size());
	}
	catch ( std::exception e )
	{
		std::cout << e.what() << std::endl;
		return false;
	}

	return false;
}
