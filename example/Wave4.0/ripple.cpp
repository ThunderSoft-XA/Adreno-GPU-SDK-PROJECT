/*
 * ripple.cpp
 *
 *  Created on: Apr 21, 2020
 *      Author: little
 */

#include "../include/ripple.h"


using namespace little;







bool Ripple::Init(const GLchar * vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param)
{
	bool status;

	Ripple::Param * p = (Ripple::Param *)param;

	assert(p);

	this->original_positions.Create(this->width * this->height);
	this->algorithms.Create(p->num, p->algorithms);

	status = Mesh::Init(vs, fs, mvp_name, ta, param);
	if (!status) return false;

	return true;
}


void Ripple::Update(double t, float dt, mat4x4 mv, void * param)
{
	if (this->selector >= this->algorithms.Length())
	{
		LOG_ERROR("The value of selector is out or range!", "");
		return;
	}

	Algorithm & algorithm = this->algorithms[this->selector];

	this->phase += algorithm.delta();

	this->ForEachVertex([&](int x, int y)
	{
		int       v_pos = this->VertexPosition(x, y);
		Vector3 & pos   = this->vertices[v_pos].position;
		Vector3 & opos  = this->original_positions[v_pos];
		GLfloat   dv    = algorithm.f(this->horizontal ? opos.x : opos.y, this->phase, algorithm);

		if (this->horizontal)
		{
			pos.y = opos.y + dv;
		}
		else
		{
			pos.x = opos.x + dv;
		}

#if 0 //Debug
		if ((x == 0 && y == 0) ||
			(x == this->width - 1 && y == this->height - 1))
		{
			PRINT_SEPARATOR();
			LOG_INFO("Phase:", this->phase);
			LOG_INFO("ROW:", x);
			LOG_INFO("COL:", y);
			LOG_INFO("POS:", v_pos);
			LOG_INFO("OX:", opos.x);
			LOG_INFO("OY:", opos.y);
			LOG_INFO("DV:", dv);
			LOG_INFO("X:", pos.x);
			LOG_INFO("Y:", pos.y);
			PRINT_SEPARATOR();
		}
#endif //Debug
	});
}


void Ripple::OnDraw()
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


void Ripple::ResetData()
{
	Mesh::ResetData();

	auto mapping = [](float v, float range)
	{
		float f = v / range;

		return f;
	};

//	PRINT_SEPARATOR();
//	LOG_INFO("Width:", this->width);
//	LOG_INFO("Height:", this->height);
//	PRINT_SEPARATOR();

	this->ForEachVertex([&](int x, int y)
	{
		int v_pos = this->VertexPosition(x, y);

		// Save original positions
		this->original_positions[v_pos] = this->vertices[v_pos].position;

		// Set texture coordinates
		float rw = this->width  - 1;
		float rh = this->height - 1;

		this->vertices[v_pos].texture.x = mapping(x, rw);
		//this->vertices[v_pos].texture.y = mapping(rw - y, rw);
		this->vertices[v_pos].texture.y = mapping(y, rw);

//		PRINT_SEPARATOR();
//		LOG_INFO("X:", x);
//		LOG_INFO("Y:", y);
//		LOG_INFO("TX:", this->vertices[v_pos].texture.x);
//		LOG_INFO("TY:", this->vertices[v_pos].texture.y);
//		PRINT_SEPARATOR();
	});
}


void Ripple::ColorizeVertex(int x, int y, int pos, bool gradient)
{
	this->vertices[pos].color.r = 1.0f;
	this->vertices[pos].color.g = 1.0f;
	this->vertices[pos].color.b = 1.0f;
}


void Ripple::Dump(void) const
{
#ifndef NDEBUG
	PRINT_SEPARATOR();
	LOG_INFO("Direction:", this->GetDirection() ? "Horizontal" : "Vertical");
	LOG_INFO("AlgorithmIndex:", this->GetSelector());
	PRINT_SEPARATOR();
#endif //NDEBUG
}



























