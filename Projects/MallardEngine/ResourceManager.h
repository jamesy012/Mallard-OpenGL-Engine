#pragma once

#include "DLLBuild.h"

class IResource;
#include <map>
#include <vector>


class  ResourceManager {
public:
	struct ResourceReference {
		IResource* resource;//this is the main version of the resource, everything else copys from this one
		unsigned int resourceCount = 0;
	};

	typedef std::vector<ResourceReference*> mapData;
	typedef std::map<unsigned int, mapData*> resourceMap;

	DLL_BUILD static bool loadResource(IResource* a_Resource);
	DLL_BUILD static void removeResource(IResource* a_Resource);

	DLL_BUILD static ResourceReference* getMainResource(IResource* a_Resource);

	DLL_BUILD static void deleteLeftOverResources();
private:
	static void addResource(ResourceReference* a_ResourceRef);
	static resourceMap m_ResourceList;
};
