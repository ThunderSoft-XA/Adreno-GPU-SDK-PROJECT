/*
 * firework.h
 *
 *  Created on: Dec 9, 2019
 *      Author: little
 */

#ifndef LIB_INCLUDE_FIREWORK_H_
#define LIB_INCLUDE_FIREWORK_H_

#include "particle.h"




namespace little
{



class Firework : public Drawable
{
public:
	Firework(unsigned int num_particles, Particle * particles) :
		num_particles(num_particles),
		num_active_particles(0),
		particles(particles),
		particle_size(0.26f),
		velocity_coef({0.3f, 0.3f, 0.1f}),
		velocity_z_basis(1.6f),
		life_span(8.0f),
		min_age(0.f),
		friction(0.6f)
	{
		FOOTPRINT();

		assert(this->particles);
		assert(this->num_particles >= 1);

		uint32_t num_vertices = this->num_particles * Particle::Vertices::Count();

#ifdef ENABLE_PARTICLE_SORTING
		this->indices.Create(this->num_particles);
		assert((bool)this->indices);
#endif //ENABLE_PARTICLE_SORTING

		this->vertices = new Vertex[num_vertices]();
		assert(this->vertices);

		LOG_INFO("NumOfVertices:", num_vertices);
	}

	~Firework()
	{
		FOOTPRINT();

		SAFE_DEL_ARRAY(this->vertices);
	}

	struct Param
	{
		Vector3 position;
		float   particle_size;
	};

public:
	virtual bool Init(const GLchar * vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param);
	virtual void Update(double t, float dt, mat4x4 mv, void * param);

protected:
	virtual void OnDraw();

public:
	uint32_t NumOfParticles(void) const { return this->num_particles; }

	const Vector3 & InitialPosition(void) const    { return this->initial_position; }
	float           LifeSpan(void)        const    { return this->life_span;        }
	float           MinAge(void)          const    { return this->min_age;          }
	float           Friction(void)        const    { return this->friction;         }
	float           ParticleSize(void)    const    { return this->particle_size;    }
	const Vector3 & VelocityCoef(void)    const    { return this->velocity_coef;    }
	float           VelocityZBasis(void)  const    { return this->velocity_z_basis; }

	void SetInitialPosition(const Vector3 & v) { this->initial_position = v; }
	void SetLifeSpan(float v)                  { this->life_span        = v; }
	void SetMinAge(float v)                    { this->min_age          = v; }
	void SetFriction(float v)                  { this->friction         = v; }
	void SetParticleSize(float v)              { this->particle_size    = v; }
	void SetVelocityCoef(const Vector3 & v)    { this->velocity_coef    = v; }
	void SetVelocityZBasis(float v)            { this->velocity_z_basis = v; }

	operator Vertex*() const           { return this->vertices; }

	void DumpVertices() const;

protected:
	virtual void InitParticle(Particle & particle, float dt);
	virtual void UpdateParticle(Particle & particle, float dt);

protected:
	float BirthInterval(void) const    { return this->life_span/this->num_particles; }
	float MaxDeltaTime(void)  const    { return this->BirthInterval() * 0.5f;         }

protected:
	uint32_t   num_particles;
	uint32_t   num_active_particles;
	Particle * particles;
	Vertex   * vertices;

#ifdef ENABLE_PARTICLE_SORTING
	IntIndexArray indices;
#endif //ENABLE_PARTICLE_SORTING

	Vector3 initial_position;
	float   life_span;
	float   min_age;
	float   friction;
	float   particle_size;
	Vector3 velocity_coef;
	float   velocity_z_basis;
};



}



#endif /* LIB_INCLUDE_FIREWORK_H_ */
