#pragma once

class IRenderable {
public:
	virtual void draw() = 0;
	virtual void drawInstance(unsigned int a_Amount) = 0;
};