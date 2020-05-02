/*
 * image_text.cpp
 *
 *  Created on: Dec 24, 2019
 *      Author: little
 */
#include "../include/math_tiny.h"
#include "../include/firework.h"
#include "../include/linmath.h"
#include "../include/physics.h"




using namespace gl_little;




namespace little
{



bool Firework::Init(const GLchar * vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param)
{
	Firework::Param * p = (Firework::Param *)param;

	assert(p);

	bool status;

	status = Drawable::Init(vs, fs, mvp_name, ta, param);
	if (!status) return false;

	this->initial_position = p->position;
	this->particle_size    = p->particle_size;

    return true;
}

/**
 * (0,1)   (1,1)
 * 3-------2
 * |      /|
 * |2nd  / |
 * |    /  |
 * |   /   |
 * |  /    |
 * | / 1st |
 * |/      |
 * 0-------1
 * (0,0)   (1,0)
 */
void Firework::Update(double t, float dt, mat4x4 mv, void * param)
{
	float max_delta_time = this->MaxDeltaTime();
	float birth_interval = this->BirthInterval();

	for (float delta; dt > 0.f; dt -= delta)
	{
		delta = dt > max_delta_time ? max_delta_time : dt;

		for (int i = 0; i < this->NumOfParticles(); i++)
		{
			this->UpdateParticle(this->particles[i], delta);
		}

		this->min_age += delta;

		while (this->min_age >= birth_interval)
		{
			this->min_age -= birth_interval;

			for (int i = 0; i < this->NumOfParticles(); i++)
			{
				Particle & p = this->particles[i];

				if (!p.Active())
				{
					this->InitParticle(p, this->MinAge());
					this->UpdateParticle(p, this->MinAge());

					break;
				}
			}
		}
	}

#ifdef ENABLE_PARTICLE_SORTING
	if (this->depth_test)
	{
		this->depth_test = false;
	}

	Array<int>::Compare compare = [&](const int & idx1, const int & idx2)
	{
		Particle & p1 = this->particles[idx1];
		Particle & p2 = this->particles[idx2];

		assert(p1.Active() && p2.Active());

		//Vector3 origin = {0.f, 0.f, 0.f};
		Vector3 pos1   = p1.GetVertex().position * mv;
		Vector3 pos2   = p2.GetVertex().position * mv;

		bool  result = true;
		float error  = 1.6;

		if (pos1.z < 0 && pos2.z < 0)
			result = pos1.z > pos2.z;
		else if (pos1.z < 0)
			result = false;
		else if (pos2.z < 0)
			result = true;
		else
			result = pos1.z > pos2.z;

//		PRINT_SEPARATOR();
//		M4x4_DUMP(mv);
//		pos1.Dump();
//		pos2.Dump();
//		LOG_INFO("Result:", result);
//		PRINT_SEPARATOR();

		return result;
	};

	// Reset the number of active particles
	this->num_active_particles = 0;
	for (int i = 0; i < this->NumOfParticles(); i++)
	{
		Particle & p = this->particles[i];

		if (!p.Active()) continue;

		this->indices[this->num_active_particles++] = i;
	}

	Array<int> active_indices(this->indices, this->num_active_particles);

	static uint32_t _count = 0;

	if (++_count % 1 == 0)
	{
		_count = 0;
		active_indices.Sort(compare);
	}

	ParallelView        parallel(this->particle_size, this->particle_size, mv);

	Particle::Vertices *pv = (Particle::Vertices*)this->vertices;

	for (int i = 0; i < this->num_active_particles; i++, pv++)
	{
		Particle & p = this->particles[this->indices[i]];

		pv->SyncWith(p, parallel);
	}
#else //!ENABLE_PARTICLE_SORTING
	ParallelView        parallel(this->particle_size, this->particle_size, mv);

	Particle           *p  = this->particles;
	Particle::Vertices *pv = (Particle::Vertices*)this->vertices;

	// Reset the number of active particles
	this->num_active_particles = 0;
	for (int i = 0; i < this->NumOfParticles(); i++, p++)
	{
		if (!p->Active()) continue;

		pv->SyncWith(*p, parallel);

		this->num_active_particles++;
		pv++;
	}
#endif //ENABLE_PARTICLE_SORTING
}

void Firework::OnDraw()
{
    unsigned int num_vertices_per_particle = Particle::Vertices::Count();

	GLsizeiptr num_vertices = this->num_active_particles * num_vertices_per_particle;
	GLsizeiptr data_size    = num_vertices * sizeof(Vertex);

	glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, this->vertices);

    for (int i = 0; i < this->num_active_particles; i++)
    {
    	glDrawArrays(GL_TRIANGLE_FAN, i * num_vertices_per_particle, num_vertices_per_particle);
    }
}

void Firework::DumpVertices() const
{
#ifndef NDEBUG
	unsigned int count = this->num_active_particles * Particle::Vertices::Count();
	if (count == 0) return;
	PRINT_SEPARATOR();
	for (int i = 0; i < count; i++)
	{
		LOG_INFO("Index:", i);
		this->vertices[i].Dump();
	}
	PRINT_SEPARATOR();
#endif  //NDEBUG
}

void Firework::InitParticle(Particle & particle, float dt)
{
	Vertex  & vertex   = particle.GetVertex();
	Vector3 & velocity = particle.GetVelocity();

	vertex.position = this->initial_position;

	particle.SetLife(1.0f);
	particle.SetActive(true);
	particle.SetAngle(2 * M_PI * Random());
	particle.Colorize(dt);

	double t      = GetTime();
	double period = 2 * M_PI / this->life_span;
	float  scale  = (0.9f + 0.1f * (float)(sin(period * t) + sin(period / 2.6f * t)));
	float  angle  = particle.Angle();

	Vector3 v = { (float)cos(angle), (float)sin(angle), Random() };

	velocity    = this->velocity_coef * v;
	velocity.z += this->velocity_z_basis;
	velocity   *= scale;
}

void Firework::UpdateParticle(Particle & particle, float dt)
{
	// Ignore inactive particles
	if (!particle.Active()) return;

	// Particle is growing up
	particle.GrowUp(dt * (1.f / this->life_span));

	if (particle.Life() <= 0.f)
	{
		particle.SetActive(false);
		return;
	}

	Vector3 & velocity = particle.GetVelocity();

	// Apply the gravity
	// vv = vv - g * t
	velocity.z -= 0.1 * VerticalVelocity(dt);

	// Update position
	particle.Position(dt);

	// Bounce
	if (velocity.z < 0.f)
	{
		float    phs    = this->particle_size / 2.0f;
		Vertex & vertex = particle.GetVertex();

		if (vertex.position.z <= phs)
		{
			velocity.z        *= -this->friction;
			vertex.position.z  = phs + this->friction * (phs - vertex.position.z);
		}
	}
}



} // namespace little


