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
		m_CanDelete = false;
	}

	DLL_BUILD virtual ~IResource() {
		std::cout << "RESOURCE DECONSTUCTOR\n";
		assert(m_CanDelete);
	}

	DLL_BUILD void load(const char* a_FileName) {
		m_FileName = a_FileName;
		m_IsMainResource = ResourceManager::loadResource(this);
	}

	DLL_BUILD void unload() {
		std::cout << "RESOURCE UNLOAD\n";
		ResourceManager::removeResource(this);
		//if (!m_IsMainResource) {
		//	m_CanDelete = true;
		//	resourceUnload();
		//	delete this;
		//}
	}

	DLL_BUILD virtual unsigned int getResourceType() const = 0;
	DLL_BUILD const char* getFilename() const {
		return m_FileName.c_str();
	}

protected:
	//first load of this file
	DLL_BUILD virtual void resourceLoad() = 0;
	//loading this file from a already loaded version
	DLL_BUILD virtual void resourceCopy(IResource* a_Resource) = 0;
	//unloads this resource from memory
	DLL_BUILD virtual void resourceUnload() = 0;

	std::string m_FileName;

	bool m_IsMainResource = false;

	bool m_CanDelete = false;
};