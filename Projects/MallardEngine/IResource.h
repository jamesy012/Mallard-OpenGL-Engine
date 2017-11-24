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
	//sets up basic information for this IResource
	IResource() {
		m_Resource_CanDelete = true;
		m_Resource_LoadOveride = m_Resource_IsMain = false;
	}

	//checks to see if we should have deleted this object or not
	//will delete the memory from the pointer
	virtual ~IResource() {
		assert(m_Resource_CanDelete);
	}

	//will start the loading process for this resource
	//handled by ResourceManager
	void load(const char* a_FileName) {
		m_Resource_FileName = a_FileName;
		ResourceManager::loadResource(this);
	}

	//will use that resources file name to load
	//handled by ResourceManager
	void load(IResource* a_Resource) {
		m_Resource_FileName = a_Resource->m_Resource_FileName;
		ResourceManager::loadResource(this);
	}

	//Call this instead of delete
	//deleting will be handled by resource manager
	void unload() {
		ResourceManager::removeResource(this);
		//if (!m_Resource_IsMain) {
		//	m_Resource_CanDelete = true;
		//	resourceUnload();
		//	delete this;
		//}
	}
	//the filepath that was given when the file was loaded
	const char* getFilename() const {
		return m_Resource_FileName.c_str();
	}
	//returns the type of the resource
	virtual unsigned int getResourceType() const = 0;
protected:
	///functions to overwrite when implementing IResource into a class
	//first load of this file
	//true for if the resource loaded correctly
	virtual bool resourceLoad() = 0;
	//loading this file from a already loaded version
	virtual void resourceCopy(IResource* a_Resource) = 0;
	//unloads this resource from memory
	//MIGHT BE UNUSED, still called tho
	virtual void resourceUnload() = 0;
	//returns a new copy of the class
	//eg return new Texture();
	virtual IResource* resourceCreate() = 0;


	//data
	std::string m_Resource_FileName;

	bool m_Resource_IsMain = false;

	bool m_Resource_CanDelete = false;

	//the loading has already been done so the program wont run
	bool m_Resource_LoadOveride = true;
};