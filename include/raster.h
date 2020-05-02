/*
 * raster.h
 *
 *  Created on: Dec 27, 2019
 *      Author: little
 */
#ifndef LIB_INCLUDE_RASTER_H_
#define LIB_INCLUDE_RASTER_H_
#include "gl_render.h"
#include "math_tiny.h"
#include "particle.h"


using namespace gl_little;


namespace little
{


#define EN_LETTER_WIDTH  5
#define EN_LETTER_HEITHT 7
typedef const unsigned char EN_LETTER[EN_LETTER_HEITHT][EN_LETTER_WIDTH];

template <class T> class Stencil
{
public:
	Stencil(T & grid, uint32_t width, uint32_t height);

public:
	const PointerArray<Vector3> & Positions() const { return this->positions; }

protected:
	void ForEachElement(std::function<void (uint32_t, uint32_t)> callback)
	{
		for (uint32_t i = 0; i < this->height; i++)
		{
			for (uint32_t j = 0; j < this->width; j++)
			{
				if (callback) callback(i, j);
			}
		}
	}

	void ForEachNonZeroElement(std::function<void (uint32_t, uint32_t)> callback)
	{
		if (!callback)
		{
			LOG_ERROR("Callback is null!", "");

			return;
		}

		auto checker = [&](uint32_t row, uint32_t col)
		{
			const unsigned char c = (this->grid)[row][col];

			if (c > 0)
			{
				callback(row, col);
			}
		};

		this->ForEachElement(checker);
	}

	uint32_t NumOfNonZeroElements(void);

protected:
	/*
	 * T * grid; // (*grid)[][]
	 */
	T        & grid;
	uint32_t   width;
	uint32_t   height;

	PointerArray<Vector3> positions;
};


class Raster : public Drawable
{
public:
	Raster() :
		scale(0.2f),
		num_particles(0),
		particles(0),
		vertices(0),
		particle_size(0.2f),
		time_bias(0.0f),
		discolor(false)
	{
		FOOTPRINT();
	}

	virtual ~Raster()
	{
		SAFE_DEL_ARRAY(this->particles);
		SAFE_DEL_ARRAY(this->vertices);
	}

	struct Param
	{
		const char * s;

		Vector3 initial_position;
		float   scale;
		float   particle_size;
		float   time_bias;

		bool    discolor;
	};

public:
	virtual bool Init(const GLchar * vs, const GLchar* fs, const GLchar * mvp_name, TextureArray * ta, void * param);
	virtual void Update(double t, float dt, mat4x4 mv, void * param);

protected:
	virtual void OnDraw();

public:
	Vector3  InitialPosition(void)          { return this->initial_position; }

	float    GetScale(void)           const { return this->scale;            }
	void     SetScale(float v)              { this->scale = v;               }

	uint32_t NumOfParticles(void)     const { return this->num_particles;    }
	float    ParticleSize(void)       const { return this->particle_size;    }
	void     SetParticleSize(float v)       { this->particle_size = v;       }

	bool     Discolor(void)           const { return this->discolor;         }
	void     EnableDiscolor(bool v)         { this->discolor = v;            }

protected:
	bool CreateParticles();

protected:
	PointerArray<Stencil<EN_LETTER>> stencils;

	Vector3    initial_position;
	float      scale;

	uint32_t   num_particles;
	Particle * particles;
	Vertex   * vertices;
	float      particle_size;

	float      time_bias;
	bool       discolor;
};



} //namespace little



#endif /* LIB_INCLUDE_RASTER_H_ */
