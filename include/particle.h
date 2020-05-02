/*
 * particle.h
 *
 *  Created on: Dec 9, 2019
 *      Author: little
 */

#ifndef LIB_INCLUDE_PARTICLE_H_
#define LIB_INCLUDE_PARTICLE_H_

#include <assert.h>




#include "gl_render.h"



using namespace gl_little;

namespace little
{



class Particle
{
public:
	Particle() :
		life(0.0f),
		active(false),
		angle(0)
	{
		id = count++;

		//FOOTPRINT();
		//this->Dump();
	}

	virtual ~Particle()
	{

	}

public:
	Particle & operator = (const Particle & p);

public:
	const Vertex  & GetVertex()   const { return this->vertex;   }
	const Vector3 & GetVelocity() const { return this->velocity; }

	Vertex  & GetVertex()   { return this->vertex;   }
	Vector3 & GetVelocity() { return this->velocity; }

	float Life(void)   const { return this->life;   }
	bool  Active(void) const { return this->active; }
	float Angle(void)  const { return this->angle;  }

	void  SetLife(float v)   { this->life   = v; }
	void  SetActive(bool v)  { this->active = v; }
	void  SetAngle(float v)  { this->angle  = v; }

	void  GrowUp(float dv)   { this->life -= dv; }

	void  Dump() const
	{
#ifndef NDEBUG
		PRINT_SEPARATOR();
		LOG_INFO("ID:", id);
		this->vertex.Dump();
		this->velocity.Dump(false);
		std::cout << "Life:" << this->life << ", Active:" << this->active << std::endl;
		PRINT_SEPARATOR();
#endif  //NDEBUG
	}

public:
	virtual void Colorize(float dt, float bias = 0.0f);
	virtual void Position(float dt) { this->vertex.position += this->velocity * dt; }

protected:
	int     id;
	Vertex  vertex;
	Vector3 velocity;

	float   life;
	bool    active;
	float   angle;

	static int count;

public:
	class Vertices
	{
	private:
		Vertices()
		{

		}

	public:
		Vertex v0, v1, v2, v3;

	public:
		void SyncWith(const Particle & particle, const ParallelView & parallel);

	public:
		static uint32_t Count() { return sizeof(Vertices) / sizeof(Vertex); }
	};
};

inline Particle & Particle::operator =(const Particle & p)
{
	this->id       = p.id;
	this->vertex   = p.vertex;
	this->velocity = p.velocity;
	this->life     = p.life;
	this->active   = p.active;
	this->angle    = p.angle;

	return *this;
}



class AnisotropicParticle : public Particle
{
public:
	virtual void Colorize(float dt, float bias = 0.0f);
};



} // namespace little



#endif /* LIB_INCLUDE_PARTICLE_H_ */
