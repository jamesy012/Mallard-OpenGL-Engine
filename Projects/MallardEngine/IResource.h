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

class DLL_BUILD IResource {
	friend ResourceManager;

public:
	IResource() {
		std::cout << "RESOURCE CONSTUCTOR\n";
		m_CanDelete = false;
	}

	virtual ~IResource() {
		std::cout << "RESOURCE DECONSTUCTOR\n";
		assert(m_CanDelete);
	}

	void load(const char* a_FileName) {
		m_FileName = a_FileName;
		m_IsMainResource = ResourceManager::loadResource(this);
	}

	void unload() {
		std::cout << "RESOURCE UNLOAD\n";
		ResourceManager::removeResource(this);
		//if (!m_IsMainResource) {
		//	m_CanDelete = true;
		//	resourceUnload();
		//	delete this;
		//}
	}

	virtual unsigned int getResourceType() const = 0;
	const char* getFilename() const {
		return m_FileName.c_str();
	}

protected:
	//first load of this file
	virtual void resourceLoad() = 0;
	//loading this file from a already loaded version
	virtual void resourceCopy(IResource* a_Resource) = 0;
	//unloads this resource from memory
	virtual void resourceUnload() = 0;

	std::string m_FileName;

	bool m_IsMainResource = false;

	bool m_CanDelete = false;
};