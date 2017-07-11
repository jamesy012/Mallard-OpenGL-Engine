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
		mainResource->m_Resource_FileName = a_Resource->m_Resource_FileName;
		mainResource->resourceLoad();

		mainReference = new ResourceReference();
		mainReference->resource = mainResource;

		addResource(mainReference);
		createdResource = true;
	}
	//there is a resource to copy from, so lets do that
	a_Resource->resourceCopy(mainReference->resource);
	mainReference->resourceCount++;

	return createdResource;
}

void ResourceManager::removeResource(IResource * a_Resource) {
	ResourceReference* mainReference = getMainResource(a_Resource);
	if (mainReference == nullptr) {
		delete a_Resource;
		return;
	}
	mainReference->resourceCount--;

	a_Resource->resourceUnload();
	a_Resource->m_Resource_CanDelete = true;
	delete a_Resource;
	a_Resource = nullptr;

	if (mainReference->resourceCount == 0) {
		//m_ResourceList[mainReference->resource->getResourceType()]
		IResource* resource = mainReference->resource;
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
	mapData* resourceObjects;
	resourceObjects = m_ResourceList[a_ResourceRef->resource->getResourceType()];

	if (resourceObjects == nullptr) {
		resourceObjects = new mapData();
	}

	resourceObjects->push_back(a_ResourceRef);
	
	m_ResourceList[a_ResourceRef->resource->getResourceType()] = resourceObjects;

}
