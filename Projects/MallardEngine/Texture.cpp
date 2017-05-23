#include "Texture.h"

#include <iostream>

#include <stb_image.h>

Texture::Texture() {
	std::cout << "TEXTURE CREATE\n";
}


Texture::~Texture() {
	std::cout << "TEXTURE DECONSTUCTOR\n";
}

unsigned int Texture::getResourceType() const {
	return ResourceTypes::RESOUCE_TEXTURE;
}

void Texture::resourceLoad() {
	std::cout << "TEXTURE LOAD\n";
}

void Texture::resourceCopy(IResource * a_Resource) {
	std::cout << "TEXTURE COPY\n";
}

void Texture::resourceUnload() {
	std::cout << "TEXTURE UNLOAD\n";
}

