/*
 * mesh.cpp
 *
 *  Created on: Nov 21, 2019
 *      Author: little
 */
#include "../include/mesh.h"
#include <assert.h>
#include "../include/math_tiny.h"




namespace little
{



bool Mesh::Init(const GLchar * vs, const GLchar* fs, const GLchar * mvp_name, TextureArray * ta, void * param)
{
	Mesh::Param * p = (Mesh::Param *)param;
	bool          status;

	assert(p);

	this->SetScale(p->scale);
	status = Drawable::Init(vs, fs, mvp_name, ta, param);

	uint32_t num = this->width * this->height;

	this->vertices.Create(num);
	this->indexes.Create(this->NumOfIndexes());

	if (!this->vertices || !this->indexes)
	{
		LOG_ERROR("Mesh::Init()", "Out of memory");

		return false;
	}

	this->ResetData();

	return true;
}

void Mesh::OnDraw()
{
	GLsizeiptr   data_size = (GLsizeiptr)(*this);
    void       * data      = this->GetVertices();

	glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, data);

	if (GL_POINTS == this->mode)
	{
		glDrawArrays(this->mode, 0, this->vertices.Length());
	}
	else
	{
		glDrawElements(this->mode, this->NumOfIndexes(), GL_UNSIGNED_SHORT, this->GetIndexes());
	}
}

void Mesh::Dump() const
{
#ifndef NDEBUG
	PRINT_SEPARATOR();
	LOG_INFO("MeshWidth:", this->GetWidth());
	LOG_INFO("MeshHeight:", this->GetHeight());
	LOG_INFO("NumOfVertices:", this->vertices.Length());
	LOG_INFO("SizeOfVertices:", (GLsizeiptr)this);
	LOG_INFO("NumberOfIndexes:", this->NumOfIndexes());
	PRINT_SEPARATOR();
#endif //NDEBUG
}

void Mesh::DumpIndexes() const
{
#ifndef NDEBUG
	for (int nc = this->GetWidth() * 2 + 2, i = 0; i < this->NumOfIndexes(); i++)
	{
		std::cout << this->GetIndexes()[i] << " ";
		if ((i % nc) == nc - 1)
			std::cout << std::endl;
	}
	std::cout << std::endl;
#endif //NDEBUG
}

void Mesh::DumpVertex(int pos) const
{
#ifndef NDEBUG
	this->vertices[pos].Dump();
#endif //NDEBUG
}

void Mesh::ColorizeVertex(int x, int y, int pos, bool gradient)
{
	const int M = 2;
	int rx = x % M;
	int ry = y % M;

	if (gradient)
	{
		this->vertices[pos].color.r = Normalize(x, 0, this->width, true);
	}
	else
	{
		if ((1 == rx || 2 == rx) && (0 == ry || 3 == ry))
		{
			this->vertices[pos].color.r = 0.0f;
		}
		else
		{
			this->vertices[pos].color.r = 1.0f;
		}
	}
	this->vertices[pos].color.g = Normalize(y, 0, this->height, true);
	this->vertices[pos].color.b = 1 - (Normalize(x, 0, this->width, true) + Normalize(y, 0, this->height, true)) / 2.f;
}

void Mesh::ForEachVertex(std::function<void (int, int)> callback)
{
	if (!callback)
	{
		LOG_ERROR("Parameter callback is null", "");

		return;
	}

	for (int y = 0; y < this->height; y++)
	{
		for (int x = 0; x < this->width; x++)
		{
			callback(x, y);
		}
	}
}

void Mesh::ResetData()
{
	this->ForEachVertex([&](int x, int y)
	{
		int v_pos = this->VertexPosition(x, y);

		this->vertices[v_pos].position.x = this->scale * ZeroCenterMapping(x, this->width);
		this->vertices[v_pos].position.y = this->scale * ZeroCenterMapping(y, this->height);
		this->UpdateZvalue(x, y, v_pos);
		this->ComputeIndex(x, y);
		this->ColorizeVertex(x, y, v_pos, true);

#if 0
		this->DumpVertex(v_pos);
#endif
	});
}

void Mesh::ComputeIndex(int x, int y)
{
	// Populates indexes
	if (y < this->height - 1)
	{
		int i_pos = 2 * this->VertexPosition(x, y);

		if (y > 0) i_pos += (y * 2);

		/**
		 * Next
		 * |
		 * +-------<-------+
		 *                 |
		 * 3-------4-------5 Degenerate/
		 * |      /|      /|
		 * | 1st / | 3rd / |
		 * |    /  |    /  |
		 * |   /   |   /   |
		 * |  /    |  /    |
		 * | / 2nd | / 4th |
		 * |/      |/      |
		 * 0-------1-------2
		 */
		// First triangle
		this->indexes[i_pos + 0] = (y)		* this->width + x;		// Current point
		this->indexes[i_pos + 1] = (y + 1)	* this->width + x;		// Upper

		if (this->width - 1 == x && y < this->height - 2)
		{
			// Regenerate triangles
			this->indexes[i_pos + 2] = this->indexes[i_pos + 1];
			this->indexes[i_pos + 3] = (y + 1) * this->width;		// The first point of next row
		}
	}
}



bool AnimatedMesh::Init(const GLchar * vs, const GLchar* fs, const GLchar * mvp_name, TextureArray * ta, void * param)
{
	bool status;

	status = Mesh::Init(vs, fs, mvp_name, ta, param);
	if (!status) return false;

	this->InitPressure();
	this->velocity.x.Reset();
	this->velocity.y.Reset();

	return true;
}

void AnimatedMesh::Update(double t, float dt, mat4x4 mv, void * param)
{
	this->frame_count++;

#ifndef NDEBUG
	if (this->frame_count % 1000 == 0)
	{
		LOG_INFO("FrameCount:", this->frame_count);
	}
#endif  //!NDEBUG

	if (this->frame_count >= this->max_frame_num)
	{
		this->frame_count = 0;

		if (this->reset_data)
		{
			this->ResetData();
		}
	}

	double current_time	= GetTime();
	double elapsed_time	= current_time - this->last_time;

	this->last_time = current_time;

//	FOOTPRINT();
#ifndef NDEBUG
	double start_time = GetTime();
#endif //!NDEBUG

	while (elapsed_time > 0.f)
	{
		this->delta_time = elapsed_time > this->MaxDeltaTime ? this->MaxDeltaTime : elapsed_time;
		elapsed_time -= this->delta_time;
		this->ComputePressure();
	}

#ifndef NDEBUG
	double loop_time = GetTime() - start_time;

	if (loop_time > 10 * this->MaxDeltaTime)
	{
		LOG_INFO("LoopTime:", loop_time);
	}
#endif //!NDEBUG

	this->ApplyPressure();
}

void AnimatedMesh::ResetData()
{
	Mesh::ResetData();

	this->InitPressure();
	this->velocity.x.Reset();
	this->velocity.y.Reset();
}

void AnimatedMesh::InitPressure()
{
	int cx = this->width / 2;
	int cy = this->height / 2;

	FOOTPRINT();

	// Initialize pressures
	this->ForEachVertex([&](int x, int y)
	{
		double d = Distance2D(x, y, cx, cy);

		if (d < cx * 0.2)
		{
			this->pressure(x, y) = cos(d * ((2 * M_PI) / (double)(this->PressurePeriod)));
		}
		else
		{
			this->pressure(x, y) = 0.0;
		}
	});
}

void AnimatedMesh::ComputePressure(void)
{
    double time_step = this->delta_time * this->TimeStepMultiple;
    double accel_x = 0;
    double accel_y = 0;

    //FOOTPRINT();

    this->ForEachVertex([&](int x, int y)
	{
		/** Compute accelerations
		 *  An acceleration is the difference between two adjacent pressures
		 *  along the coordinate axis in X/Y direction.
		 */
		accel_x = this->pressure(x, y) - this->pressure((x + 1) % this->width, y);
		accel_y = this->pressure(x, y) - this->pressure(x, (y + 1) % this->height);

		/** Compute speeds
		 *  a = dv/dt
		 */
		this->velocity.x(x, y) += accel_x * time_step;
		this->velocity.y(x, y) += accel_y * time_step;

		// Compute pressure
		double v;

		auto fv = [](int x, int y, DualMatrix & mv, double time_step)
		{
			return (mv.x(x - 1, y) - mv.x(x, y) + mv.y(x, y - 1) - mv.y(x, y)) * time_step;
		};

		if (x >= 1 && y >= 1)
		{
			v = fv(x, y, this->velocity, time_step);
		}
		else if (x >= 1)
		{
			v = fv(x, y + 1, this->velocity, time_step) * 0.5;
		}
		else if (y >= 1)
		{
			v = fv(x + 1, y, this->velocity, time_step) * 0.5;
		}
		else // x < 1 && y < 1
		{
			v = fv(x + 1, y + 1, this->velocity, time_step) * 0.5;
		}

		this->pressure(x, y) += v;
	});
}

void AnimatedMesh::ApplyPressure(void)
{
    int pos;

    //FOOTPRINT();

#ifndef NDEBUG
    //GLfloat max_z = -1e+10, min_z = 1e+10;
#endif //!NDEBUG

    this->ForEachVertex([&](int x, int y)
	{
		pos = this->VertexPosition(x, y);

		this->vertices[pos].position.z += (float)this->pressure(x, y) * this->PressureCoefficient;

#ifndef NDEBUG
		//max_z = this->vertices[pos].position.z > max_z ? this->vertices[pos].position.z : max_z;
		//min_z = this->vertices[pos].position.z < min_z ? this->vertices[pos].position.z : min_z;
#endif //!NDEBUG
	});

#ifndef NDEBUG
    //LOG_INFO("MaxZ:", max_z);
    //LOG_INFO("MinZ:", min_z);
#endif //!NDEBUG
}



void FilteringMesh::UpdateZvalue(int x, int y, int pos)
{
#if 0
	float c = 6.f;

    float dx = this->vertices[pos].x * c;
    float dy = this->vertices[pos].y * c;

    this->vertices[pos].z = sinc(dx, dy);
#else
    AnimatedMesh::UpdateZvalue(x, y, pos);
#endif
}

void FilteringMesh::ApplyPressure()
{
    int		pos	= 0;
    float	c 	= 6.f;

    //FOOTPRINT();

    this->ForEachVertex([&](int x, int y)
	{
    	pos = this->VertexPosition(x, y);

		float dx = this->vertices[pos].position.x * c + (float) (this->pressure(x, y));
		float dy = this->vertices[pos].position.y * c + (float) (this->pressure(x, y));

		this->vertices[pos].position.z = Sinc(dx, dy);
	});
}



} // namespace little
