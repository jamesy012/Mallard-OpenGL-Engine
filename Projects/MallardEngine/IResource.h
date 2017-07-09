#pragma once
#include "DLLBuild.h"

#include <iostream>//remove later
#include <string>
#include <assert.h>

#include "ResourceManager.h"

enum ResourceTypes {
	RESOUCE_MODEL,
	RESOUCE_TEXTURE,
	RESOUCE_END_TYPE_LIST
};

class  IResource {
	friend ResourceManager;

public:
	DLL_BUILD IResource() {
		std::cout << "RESOURCE CONSTUCTOR\n";
		m_Resource_CanDelete = m_Resource_LoadOveride = m_Resource_IsMain = false;
	}

	DLL_BUILD virtual ~IResource() {
		std::cout << "RESOURCE DECONSTUCTOR\n";
		assert(m_Resource_CanDelete);
	}

	DLL_BUILD void load(const char* a_FileName) {
		m_Resource_FileName = a_FileName;
		m_Resource_IsMain = ResourceManager::loadResource(this);
	}

	DLL_BUILD void unload() {
		std::cout << "RESOURCE UNLOAD\n";
		ResourceManager::removeResource(this);
		//if (!m_Resource_IsMain) {
		//	m_Resource_CanDelete = true;
		//	resourceUnload();
		//	delete this;
		//}
	}

	DLL_BUILD virtual unsigned int getResourceType() const = 0;
	DLL_BUILD const char* getFilename() const {
		return m_Resource_FileName.c_str();
	}
protected:
	//first load of this file
	DLL_BUILD virtual void resourceLoad() = 0;
	//loading this file from a already loaded version
	DLL_BUILD virtual void resourceCopy(IResource* a_Resource) = 0;
	//unloads this resource from memory
	DLL_BUILD virtual void resourceUnload() = 0;


	//data
	std::string m_Resource_FileName;

	bool m_Resource_IsMain = false;

	bool m_Resource_CanDelete = false;

	//the loading has already been done so the program wont run
	bool m_Resource_LoadOveride = true;
};