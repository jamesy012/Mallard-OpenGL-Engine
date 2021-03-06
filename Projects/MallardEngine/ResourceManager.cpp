#include "ResourceManager.h"

#include "IResource.h"

ResourceManager::resourceMap ResourceManager::m_ResourceList;

bool ResourceManager::loadResource(IResource * a_Resource) {
	ResourceReference* mainReference = getMainResource(a_Resource);

	bool createdResource = false;
	if(mainReference != nullptr){
		if (mainReference->resource == a_Resource) {
			assert(false);
			return false;//we wont add the resource again
		}
		//copy has been moved to after this for loop
		//due to the else part now creating a seperate version,
		//so we still want to copy the resource anyway
		//a_Resource->resourceCopy(mainReference->resource);
	} else {
		//a_Resource->resourceLoad();
		//create new mainResource, copy file path, and load
		IResource* mainResource = a_Resource->resourceCreate();
		mainResource->m_Resource_IsMain = true;
		mainResource->m_Resource_CanDelete = false;
		mainResource->m_Resource_FileName = a_Resource->m_Resource_FileName;

		printf("Loading Resource Start: %s\n", a_Resource->m_Resource_FileName.c_str());

		bool didLoad = mainResource->resourceLoad();

		printf("Loading Resource %s %s \n", didLoad?"Loaded":"Failed", a_Resource->m_Resource_FileName.c_str());

		if (!didLoad) {
			//failed to load
			delete mainResource;
			return false;
		}

		mainReference = new ResourceReference();
		mainReference->resource = mainResource;

		ResourceManager::addResource(mainReference);
		createdResource = true;
	}
	//there is a resource to copy from, so lets do that
	a_Resource->resourceCopy(mainReference->resource);
	a_Resource->m_Resource_CanDelete = false;

	mainReference->resourceCount++;

	return createdResource;
}

void ResourceManager::removeResource(IResource * a_Resource) {
	//get the resource
	ResourceReference* mainReference = getMainResource(a_Resource);
	//if there is no main resource
	if (mainReference == nullptr) {
		delete a_Resource;
		return;
	}
	mainReference->resourceCount--;

	//delete a_Resource
	a_Resource->resourceUnload();
	a_Resource->m_Resource_CanDelete = true;
	delete a_Resource;
	a_Resource = nullptr;

	//if that was the last resource then delete the main resource
	if (mainReference->resourceCount == 0) {
		//m_ResourceList[mainReference->resource->getResourceType()]
		IResource* resource = mainReference->resource;
		if (resource == nullptr) {
			return;
		}
		int resourceIndex = mainReference->resource->getResourceType();

		resource->resourceUnload();
		resource->m_Resource_CanDelete = true;
		delete resource;
		mainReference->resource = nullptr;

		
		//remove from list
		for (size_t i = 0; i < m_ResourceList[resourceIndex]->size(); i++) {
			if (m_ResourceList[resourceIndex]->at(i) == mainReference) {
				m_ResourceList[resourceIndex]->erase(m_ResourceList[resourceIndex]->begin() + i);
				delete mainReference;
				break;
			}
		}
	}
}

ResourceManager::ResourceReference* ResourceManager::getMainResource(IResource * a_Resource) {
	if (a_Resource == nullptr) {
		return nullptr;
	}
	mapData* resourceObjects;
	resourceObjects = m_ResourceList[a_Resource->getResourceType()];

	if (resourceObjects == nullptr) {
		return nullptr;
	}

	for (size_t i = 0; i < resourceObjects->size(); i++) {
		ResourceReference* ref = resourceObjects->at(i);
		//if (ref->resource == a_Resource) {
		//	continue;
		//}
		if (ref->resource->m_Resource_FileName == a_Resource->m_Resource_FileName) {
			return ref;
		}
	}

	return nullptr;
}

void ResourceManager::deleteLeftOverResources() {

	for (resourceMap::iterator it = m_ResourceList.begin(); it != m_ResourceList.end(); ++it) {
		//it-->first gives you the key (int)
		//it->second gives you the mapped element (vector)
		mapData* data = it->second;
		if (data == nullptr) {
			continue;
		}
		for (size_t i = 0; i < data->size(); i++) {
			ResourceReference* ref = data->at(i);
			delete ref;
		}
		delete data;
	}

}

void ResourceManager::addResource(ResourceReference * a_ResourceRef) {
	if (a_ResourceRef == nullptr || a_ResourceRef->resource == nullptr) {
		return;
	}
	mapData* resourceObjects;
	resourceObjects = m_ResourceList[a_ResourceRef->resource->getResourceType()];

	if (resourceObjects == nullptr) {
		resourceObjects = new mapData();
	}

	resourceObjects->push_back(a_ResourceRef);
	
	m_ResourceList[a_ResourceRef->resource->getResourceType()] = resourceObjects;

}
