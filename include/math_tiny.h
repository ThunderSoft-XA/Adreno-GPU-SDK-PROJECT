/*
 * math_tiny.h
 *
 *  Created on: Nov 21, 2019
 *      Author: little
 */

#ifndef INCLUDE_MATH_TINY_H_
#define INCLUDE_MATH_TINY_H_

#include <math.h>

#include "gl_types.h"


static auto ZeroCenterMapping = [](uint32_t index, uint32_t total_number, float half_range = 1.0f)
{
	if (total_number <= 1 || half_range <= 0.0f)
		return 0.0f;

	float base = (half_range * 2) / (total_number - 1);

	return index * base - half_range;
};



static auto Normalize = [](float v, float min_v, float max_v, bool positive = true)
{
	if (max_v <= min_v) return 0.f;

	float range = (max_v - min_v);

	if (positive)
	{
		return (v - min_v) / range;
	}
	else
	{
		return (v - range / 2.0f) / (range / 2.0f);
	}
};



static auto Random = [](unsigned int R = 0x10000)
{
	if (R < 0x10) R = 0x10;

	return (float)(rand() & (R - 1)) / (float)R;
};



// Euclidean distance
static inline double Distance2D(float x, float y, float x0 = 0.f, float y0 = 0.f)
{
	double dx = (double)(x - x0);
	double dy = (double)(y - y0);

	return sqrt(dx * dx + dy * dy);
};

static inline double Distance3D(float x, float y, float z, float x0 = 0.f, float y0 = 0.f, float z0 = 0.f)
{
	double dx = (double)(x - x0);
	double dy = (double)(y - y0);
	double dz = (double)(z - z0);

	return sqrt(dx * dx + dy * dy + dz * dz);
};

static inline double Distance3D(const gl_little::Vector3 & v, const gl_little::Vector3 & v0)
{
	return Distance3D(v.x, v.y, v.z, v0.x, v0.y, v0.z);
}



static auto Radian = [](double degree)
{
	return degree * M_PI / 180.0;
};



static auto T = [](float omega)
{
	return 2.0f * M_PI / omega;
};



static auto Sine = [](float t, float omega, float phase, float amplitude = 1.0f, bool positive = true)
{
	double v = amplitude * sin(omega * t + phase);

	return positive ? (v + amplitude) / (2 * amplitude) : v;
};



// Three-dimension sinc function
static auto Sinc = [](float x, float y)
{
	float v;

	if (0.f == x and 0.f == y)
	{
		v = 1.f;
	}
	else if (0.f == x)
	{
		v = sin(y) / y;
	}
	else if (0.f == y)
	{
		v = sin(x) / x;
	}
	else
	{
		v = (sin(x) / x) * (sin(y) / y);
	}

	return v;
};



#endif /* INCLUDE_MATH_TINY_H_ */
