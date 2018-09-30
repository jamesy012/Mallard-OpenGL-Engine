#pragma once
#include "DLLBuild.h"

#include <vector>

class DLL_BUILD PerllinNoise {
public:
	PerllinNoise();
	~PerllinNoise();

	void generate(int a_Seed);

	float noise(float x, float y,float z);
private:
	std::vector<int> p;
	static double Fade(double t) noexcept {
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	static double Lerp(double t, double a, double b) noexcept {
		return a + t * (b - a);
	}

	static double Grad(int hash, double x, double y, double z) noexcept {
		const int h = hash & 15;
		const double u = h < 8 ? x : y;
		const double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
};

