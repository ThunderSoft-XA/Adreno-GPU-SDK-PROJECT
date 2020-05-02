/*
 * raster.cpp
 *
 *  Created on: Dec 27, 2019
 *      Author: little
 */
#include "../include/raster.h"
#include <assert.h>
#include <string.h>






namespace little
{



static EN_LETTER A =
{
/*        0  1  2  3  4
/* 0*/	{ 0, 0, 1, 0, 0 },
/* 1*/	{ 0, 1, 0, 1, 0 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 1, 1, 1, 1 },
/* 4*/	{ 1, 0, 0, 0, 1 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 1, 0, 0, 0, 1 }
};

static EN_LETTER B =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 1, 1, 1, 0 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 1, 1, 1, 0 },
/* 4*/	{ 1, 0, 0, 0, 1 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 1, 1, 1, 1, 0 }
};

static EN_LETTER C =
{
/*        0  1  2  3  4
/* 0*/	{ 0, 1, 1, 1, 1 },
/* 1*/	{ 1, 0, 0, 0, 0 },
/* 2*/	{ 1, 0, 0, 0, 0 },
/* 3*/	{ 1, 0, 0, 0, 0 },
/* 4*/	{ 1, 0, 0, 0, 0 },
/* 5*/	{ 1, 0, 0, 0, 0 },
/* 6*/	{ 0, 1, 1, 1, 1 }
};

static EN_LETTER D =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 1, 1, 1, 0 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 0, 0, 0, 1 },
/* 4*/	{ 1, 0, 0, 0, 1 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 1, 1, 1, 1, 0 }
};

static EN_LETTER E =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 1, 1, 1, 1 },
/* 1*/	{ 1, 0, 0, 0, 0 },
/* 2*/	{ 1, 0, 0, 0, 0 },
/* 3*/	{ 1, 1, 1, 1, 1 },
/* 4*/	{ 1, 0, 0, 0, 0 },
/* 5*/	{ 1, 0, 0, 0, 0 },
/* 6*/	{ 1, 1, 1, 1, 1 }
};

static EN_LETTER F =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 1, 1, 1, 1 },
/* 1*/	{ 1, 0, 0, 0, 0 },
/* 2*/	{ 1, 0, 0, 0, 0 },
/* 3*/	{ 1, 1, 1, 1, 1 },
/* 4*/	{ 1, 0, 0, 0, 0 },
/* 5*/	{ 1, 0, 0, 0, 0 },
/* 6*/	{ 1, 0, 0, 0, 0 }
};

static EN_LETTER G =
{
/*        0  1  2  3  4
/* 0*/	{ 0, 1, 1, 1, 1 },
/* 1*/	{ 1, 0, 0, 0, 0 },
/* 2*/	{ 1, 0, 0, 0, 0 },
/* 3*/	{ 1, 0, 0, 1, 1 },
/* 4*/	{ 1, 0, 0, 0, 1 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 0, 1, 1, 1, 0 }
};

static EN_LETTER H =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 1 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 1, 1, 1, 1 },
/* 4*/	{ 1, 0, 0, 0, 1 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 1, 0, 0, 0, 1 }
};

static EN_LETTER I =
{
/*        0  1  2  3  4
/* 0*/	{ 0, 1, 1, 1, 0 },
/* 1*/	{ 0, 0, 1, 0, 0 },
/* 2*/	{ 0, 0, 1, 0, 0 },
/* 3*/	{ 0, 0, 1, 0, 0 },
/* 4*/	{ 0, 0, 1, 0, 0 },
/* 5*/	{ 0, 0, 1, 0, 0 },
/* 6*/	{ 0, 1, 1, 1, 0 }
};

static EN_LETTER J =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 1, 1, 1, 1 },
/* 1*/	{ 0, 0, 1, 0, 0 },
/* 2*/	{ 0, 0, 1, 0, 0 },
/* 3*/	{ 0, 0, 1, 0, 0 },
/* 4*/	{ 0, 0, 1, 0, 0 },
/* 5*/	{ 0, 0, 1, 0, 0 },
/* 6*/	{ 1, 1, 0, 0, 0 }
};

static EN_LETTER K =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 1 },
/* 1*/	{ 1, 0, 0, 1, 0 },
/* 2*/	{ 1, 0, 1, 0, 0 },
/* 3*/	{ 1, 1, 0, 0, 0 },
/* 4*/	{ 1, 0, 1, 0, 0 },
/* 5*/	{ 1, 0, 0, 1, 0 },
/* 6*/	{ 1, 0, 0, 0, 1 }
};

static EN_LETTER L =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 0 },
/* 1*/	{ 1, 0, 0, 0, 0 },
/* 2*/	{ 1, 0, 0, 0, 0 },
/* 3*/	{ 1, 0, 0, 0, 0 },
/* 4*/	{ 1, 0, 0, 0, 0 },
/* 5*/	{ 1, 0, 0, 0, 0 },
/* 6*/	{ 1, 1, 1, 1, 1 }
};

static EN_LETTER M =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 1 },
/* 1*/	{ 1, 1, 0, 1, 1 },
/* 2*/	{ 1, 0, 1, 0, 1 },
/* 3*/	{ 1, 0, 0, 0, 1 },
/* 4*/	{ 1, 0, 0, 0, 1 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 1, 0, 0, 0, 1 }
};

static EN_LETTER N =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 1 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 1, 0, 0, 1 },
/* 3*/	{ 1, 0, 1, 0, 1 },
/* 4*/	{ 1, 0, 0, 1, 1 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 1, 0, 0, 0, 1 }
};

static EN_LETTER O =
{
/*        0  1  2  3  4
/* 0*/	{ 0, 1, 1, 1, 0 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 0, 0, 0, 1 },
/* 4*/	{ 1, 0, 0, 0, 1 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 0, 1, 1, 1, 0 }
};

static EN_LETTER P =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 1, 1, 1, 0 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 1, 1, 1, 0 },
/* 4*/	{ 1, 0, 0, 0, 0 },
/* 5*/	{ 1, 0, 0, 0, 0 },
/* 6*/	{ 1, 0, 0, 0, 0 }
};

static EN_LETTER Q =
{
/*        0  1  2  3  4
/* 0*/	{ 0, 1, 1, 1, 0 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 0, 0, 0, 1 },
/* 4*/	{ 1, 0, 1, 0, 1 },
/* 5*/	{ 1, 0, 0, 1, 0 },
/* 6*/	{ 0, 1, 1, 0, 1 }
};

static EN_LETTER R =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 1, 1, 1, 0 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 1, 1, 1, 0 },
/* 4*/	{ 1, 0, 1, 0, 0 },
/* 5*/	{ 1, 0, 0, 1, 0 },
/* 6*/	{ 1, 0, 0, 0, 1 }
};

static EN_LETTER S =
{
/*        0  1  2  3  4
/* 0*/	{ 0, 1, 1, 1, 1 },
/* 1*/	{ 1, 0, 0, 0, 0 },
/* 2*/	{ 1, 0, 0, 0, 0 },
/* 3*/	{ 0, 1, 1, 1, 0 },
/* 4*/	{ 0, 0, 0, 0, 1 },
/* 5*/	{ 0, 0, 0, 0, 1 },
/* 6*/	{ 1, 1, 1, 1, 0 }
};

static EN_LETTER T =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 1, 1, 1, 1 },
/* 1*/	{ 0, 0, 1, 0, 0 },
/* 2*/	{ 0, 0, 1, 0, 0 },
/* 3*/	{ 0, 0, 1, 0, 0 },
/* 4*/	{ 0, 0, 1, 0, 0 },
/* 5*/	{ 0, 0, 1, 0, 0 },
/* 6*/	{ 0, 0, 1, 0, 0 }
};

static EN_LETTER U =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 1 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 0, 0, 0, 1 },
/* 4*/	{ 1, 0, 0, 0, 1 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 0, 1, 1, 1, 0 }
};

static EN_LETTER V =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 1 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 0, 0, 0, 1 },
/* 4*/	{ 1, 0, 0, 0, 1 },
/* 5*/	{ 0, 1, 0, 1, 0 },
/* 6*/	{ 0, 0, 1, 0, 0 }
};

static EN_LETTER W =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 1 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 1, 0, 0, 0, 1 },
/* 3*/	{ 1, 0, 1, 0, 1 },
/* 4*/	{ 1, 0, 1, 0, 1 },
/* 5*/	{ 0, 1, 0, 1, 0 },
/* 6*/	{ 0, 1, 0, 1, 0 }
};

static EN_LETTER X =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 1 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 0, 1, 0, 1, 0 },
/* 3*/	{ 0, 0, 1, 0, 0 },
/* 4*/	{ 0, 1, 0, 1, 0 },
/* 5*/	{ 1, 0, 0, 0, 1 },
/* 6*/	{ 1, 0, 0, 0, 1 }
};

static EN_LETTER Y =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 0, 0, 0, 1 },
/* 1*/	{ 1, 0, 0, 0, 1 },
/* 2*/	{ 0, 1, 0, 1, 0 },
/* 3*/	{ 0, 0, 1, 0, 0 },
/* 4*/	{ 0, 0, 1, 0, 0 },
/* 5*/	{ 0, 0, 1, 0, 0 },
/* 6*/	{ 0, 0, 1, 0, 0 }
};

static EN_LETTER Z =
{
/*        0  1  2  3  4
/* 0*/	{ 1, 1, 1, 1, 1 },
/* 1*/	{ 0, 0, 0, 0, 1 },
/* 2*/	{ 0, 0, 0, 1, 0 },
/* 3*/	{ 0, 0, 1, 0, 0 },
/* 4*/	{ 0, 1, 0, 0, 0 },
/* 5*/	{ 1, 0, 0, 0, 0 },
/* 6*/	{ 1, 1, 1, 1, 1 }
};

static EN_LETTER Blank =
{
/*        0  1  2  3  4
/* 0*/	{ 0, 0, 0, 0, 0 },
/* 1*/	{ 0, 0, 0, 0, 0 },
/* 2*/	{ 0, 0, 0, 0, 0 },
/* 3*/	{ 0, 0, 0, 0, 0 },
/* 4*/	{ 0, 0, 0, 0, 0 },
/* 5*/	{ 0, 0, 0, 0, 0 },
/* 6*/	{ 0, 0, 0, 0, 0 }
};

static EN_LETTER* Alphabet[] =
{
	&A, &B, &C, &D, &E, &F, &G,
	&H, &I, &J, &K, &L, &M, &N,
	&O, &P, &Q, &R, &S, &T,
	&U, &V, &W, &X, &Y, &Z
};



#ifndef NDEBUG
#define DUMP_LETTER(la) \
	do { \
		for (int i = 0; i < EN_LETTER_HEITHT; i++) \
		{ \
			for (int j = 0; j < EN_LETTER_WIDTH; j++) \
			{ \
				std::cout << (int)((la)[i][j]) << " "; \
			} \
			std::cout << std::endl; \
		} \
	} while(0)
#else  //NDEBUG
#define DUMP_LETTER(la)
#endif //!NDEBUG



template <class T>
Stencil<T>::Stencil(T & grid, uint32_t width, uint32_t height) :
	grid(grid),
	width(width),
	height(height)
{
	if (this->width < 1 || this->height < 1)
	{
		LOG_ERROR("InvalidWidth:", this->width);
		LOG_ERROR("InvalidHeight:", this->height);

		return;
	}

	uint32_t num_non_zero_elements = this->NumOfNonZeroElements();
	if (0 == num_non_zero_elements)
	{
		// No non-zero elements, just return
//		LOG_INFO("No non-zero elements.", "");

		return;
	}

	if (!this->positions.Create(num_non_zero_elements))
	{
		LOG_ERROR("Out of memory!", "");

		return;
	}

	float    hs    = this->width >= this->height ? this->width - 1 : this->height - 1;
	uint32_t count = 0;

	hs /= 2.0f;

//	LOG_INFO("NumOfNonZeroElements:", (uint32_t)this->positions);
//	LOG_INFO("HalfSize:", hs);

	/*
	 * (0,  0)---(W-1,  0)    (-1, 1)---( 1, 1)
	 *   |           |           |         |
	 *   |           |     =>    |         |
	 *   |           |           |         |
	 * (0,H-1)---(W-1,H-1)    (-1,-1)---( 1,-1)
	 */
	auto positioner = [&](uint32_t row, uint32_t col)
	{
		float h = this->width  >= this->height ? col : col + (this->height - this->width) / 2.0f;
		float v = this->height >= this->width  ? row : row + (this->height - this->width) / 2.0f;
		float x = (h  - hs) / hs;
		float y = (hs - v ) / hs;

		Vector3 * pos = new Vector3(x, y, 0.0f);

		assert(pos);
		if (!pos)
		{
			LOG_ERROR("Out of memory!", "");
			return;
		}

		this->positions[count] = pos;

//#ifndef NDEBUG
//		std::cout << count << ": (" << row << ", " << col << ")";
//		this->positions[count]->FormatDump();
//#endif  //!NDEBUG

		count++;
	};

//	DUMP_LETTER(this->grid);

	this->ForEachNonZeroElement(positioner);

	assert(count == num_non_zero_elements);
}

template <class T>
uint32_t Stencil<T>::NumOfNonZeroElements()
{
	uint32_t num = 0;

	auto counter = [&](uint32_t row, uint32_t col)
	{
		const unsigned char c = (this->grid)[row][col];

		if (c > 0)
		{
			num++;
		}
	};

	this->ForEachElement(counter);

	return num;
}



bool Raster::Init(const GLchar * vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param)
{
	bool status;

	status = Drawable::Init(vs, fs, mvp_name, ta, param);
	if (!status) return false;

	Raster::Param * p = (Raster::Param *)param;

	assert(p && p->s);

	this->initial_position = p->initial_position;
	this->scale            = p->scale;
	this->time_bias        = p->time_bias;
	this->particle_size    = p->particle_size;
	this->discolor         = p->discolor;

	uint32_t length = strlen(p->s);

//	PRINT_SEPARATOR();
//	LOG_INFO("String:", p->s);
//	LOG_INFO("StringLength:", strlen(p->s));
//	LOG_INFO("Length:", length);
//	LOG_INFO("A:", A);
//	LOG_INFO("&A:", &A);

	if (!this->stencils.Create(length))
	{
		LOG_ERROR("Out of memory!", "");

		return false;
	}

	for (int i = 0; i < (uint32_t)this->stencils; i++)
	{
		char c     = p->s[i];
		int  index = -1;

		Stencil<EN_LETTER> * stencil = 0;

		if (c >= 'A' && c <= 'Z')
		{
			// 'A' - 'Z'
			index = c - 'A';
		}
		else if (c >= 'a' && c <= 'z')
		{
			// 'a' - 'z'
			index = c - 'a';
		}

		if (index >= 0)
		{
			stencil = new Stencil<EN_LETTER>(*Alphabet[index], EN_LETTER_WIDTH, EN_LETTER_HEITHT);

//			LOG_INFO("Index:", index);
//			LOG_INFO("Letter:", c);
		}
		else
		{
			// Blank spaces or other characters
			stencil = new Stencil<EN_LETTER>(Blank, EN_LETTER_WIDTH, EN_LETTER_HEITHT);

//			LOG_ERROR("UnsupportedCharacter:", c);
		}

		if (!stencil)
		{
			LOG_ERROR("Out of memory.", "");
		}

		this->stencils[i] = stencil;
	}

//	PRINT_SEPARATOR();

	this->CreateParticles();

    return true;
}

void Raster::Update(double t, float dt, mat4x4 mv, void * param)
{
	uint32_t num_stencils = this->stencils.Length();
	uint32_t count = 0;

	for (uint32_t i = 0; i < num_stencils; i++)
	{
		float stencil_width = 1 + 1 * 1.0;
		float total_width   = stencil_width * (num_stencils - 1);
		float offset        = stencil_width * i - total_width / 2.0f;

		const PointerArray<Vector3> & positions     = this->stencils[i]->Positions();
		      uint32_t                num_positions = positions;

		for (uint32_t j = 0; j < num_positions; j++)
		{
			Particle & p = this->particles[count++];
			Vertex   & v = p.GetVertex();

			v.position    = *positions[j];
			v.position.x += offset;
			v.position   *= this->scale;
			v.position   += this->initial_position;
		}
	}

	ParallelView parallel(this->particle_size, this->particle_size, mv);
	mat4x4       inv_mv;

	Particle           *p  = this->particles;
	Particle::Vertices *pv = (Particle::Vertices*)this->vertices;

	mat4x4_invert(inv_mv, mv);

	for (uint32_t i = 0; i < this->NumOfParticles(); i++, p++, pv++)
	{
		Vertex & v   = p->GetVertex();
		vec4     pos = { v.position.x, v.position.y, v.position.z };

		v.position *= inv_mv;

		if (this->discolor)
		{
			p->Colorize(dt, this->time_bias + i + dt);
		}
		else
		{
			p->Colorize(dt, this->time_bias);
		}

		pv->SyncWith(*p, parallel);
	}
}

void Raster::OnDraw()
{
	if (!this->vertices) return;

    uint32_t num_vertices_per_particle = Particle::Vertices::Count();

	GLsizeiptr num_vertices = this->num_particles * num_vertices_per_particle;
	GLsizeiptr data_size    = num_vertices * sizeof(Vertex);

	glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, this->vertices);

    for (int i = 0; i < this->num_particles; i++)
    {
    	glDrawArrays(GL_TRIANGLE_FAN, i * num_vertices_per_particle, num_vertices_per_particle);
    }
}

bool Raster::CreateParticles()
{
	assert(!this->particles && 0 == this->num_particles);

	if (!this->stencils || 0 == (uint32_t)this->stencils)
	{
		LOG_ERROR("No any stencil", "");

		return false;
	}

	uint32_t num_stencils = this->stencils;
	this->num_particles = 0;
	for (uint32_t i = 0; i < num_stencils; i++)
	{
		this->num_particles += (uint32_t)this->stencils[i]->Positions();
	}

	PRINT_SEPARATOR();
	LOG_INFO("NumOfParticles:", this->num_particles);
	PRINT_SEPARATOR();

	this->particles = new Particle[this->num_particles];
	if (!this->particles)
	{
		LOG_ERROR("Particles:", "Out of memory.");

		this->num_particles = 0;

		return false;
	}

	uint32_t num_vertices = this->num_particles * Particle::Vertices::Count();
	this->vertices = new Vertex[num_vertices];
	if (!this->vertices)
	{
		LOG_ERROR("Vertices:", "Out of memory.");

		this->num_particles = 0;
		SAFE_DEL_ARRAY(this->particles);

		return false;
	}

	return true;
}



} // namespace little


