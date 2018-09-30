#include "Noise.h"

#include <math.h>
#include <numeric>
#include <algorithm>
#include <random>

PerllinNoise::PerllinNoise() {
}


PerllinNoise::~PerllinNoise() {
}

void PerllinNoise::generate(int a_Seed) {
	p.resize(256);
	
	// Fill p with values from 0 to 255
	std::iota(p.begin(), p.end(), 0);
	
	// Initialize a random engine with seed
	std::default_random_engine engine(a_Seed);
	
	// Suffle  using the above random engine
	std::shuffle(p.begin(), p.end(), engine);
	
	// Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}

// Compute Perlin noise at coordinates x, y
float PerllinNoise::noise(float x, float y,float z) {
	const int X = static_cast<int>(floor(x)) & 255;
	const int Y = static_cast<int>(floor(y)) & 255;
	const int Z = static_cast<int>(floor(z)) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	const double u = Fade(x);
	const double v = Fade(y);
	const double w = Fade(z);

	const int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
	const int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
								Grad(p[BA], x - 1, y, z)),
						Lerp(u, Grad(p[AB], x, y - 1, z),
							 Grad(p[BB], x - 1, y - 1, z))),
				Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
							 Grad(p[BA + 1], x - 1, y, z - 1)),
					 Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
						  Grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

