/*
 * particle.cpp
 *
 *  Created on: Dec 9, 2019
 *      Author: little
 */

#include "../include/math_tiny.h"
#include "../include/particle.h"






namespace little
{



int Particle::count = 0;



void Particle::Colorize(float dt, float bias)
{
	double t = GetTime();

	this->vertex.color.r = Sine(t, 0.22, 0.1 + bias);
	this->vertex.color.g = Sine(t, 0.67, 1.1 + bias);
	this->vertex.color.b = Sine(t, 0.95, 1.9 + bias);
}

void Particle::Vertices::SyncWith(const Particle & particle, const ParallelView & parallel)
{
	const Vector3 & position = particle.GetVertex().position;

	this->v0.color = this->v1.color = this->v2.color = this->v3.color = particle.vertex.color;

	// 0: Lower left
	this->v0.position = position + parallel.LowerLeft();
	this->v0.texture  = LL_TEX_COORD;

	// 1: Lower right
	this->v1.position = position + parallel.LowerRight();
	this->v1.texture  = LR_TEX_COORD;

	// 2: Upper right
	this->v2.position = position + parallel.UpperRight();
	this->v2.texture  = UR_TEX_COORD;

	// 3: Upper left
	this->v3.position = position + parallel.UpperLeft();
	this->v3.texture  = UL_TEX_COORD;
}



void AnisotropicParticle::Colorize(float dt, float bias)
{
	double t = GetTime();

	this->vertex.color.r = Sine(this->angle * t, 0.22, t + bias);
	this->vertex.color.g = Sine(this->angle * t, 0.67, t + bias + 1.1);
	this->vertex.color.b = Sine(this->angle * t, 0.95, t + bias + 1.9);
}



} // namespace little
