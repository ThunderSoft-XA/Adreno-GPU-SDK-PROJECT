
/*
 * image_text.cpp
 *
 *  Created on: Dec 24, 2019
 *      Author: little
 */
#define STB_IMAGE_IMPLEMENTATION

#include "../include/image.h"
#include <assert.h>




using namespace little;




bool Image::Init(const GLchar *vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param)
{
	Image::Param * p = (Image::Param *)param;

	assert(param);

	bool status;

	status = Drawable::Init(vs, fs, mvp_name, ta, param);
	if (!status) return false;

	this->anchor = p->anchor;
	this->size   = p->size;

	return true;
}


void Image::Update(double t, float dt, mat4x4 mv, void * param)
{
	ParallelView parallel(size.w, size.h, mv);

	// 0: Lower Left
	this->vertices[0].position = this->anchor + parallel.LowerLeft();
	this->vertices[0].color    = {1.0f, 0.0f, 0.0f};
	this->vertices[0].texture  = LL_TEX_COORD;

	// 1: Lower Right
	this->vertices[1].position = this->anchor + parallel.LowerRight();
	this->vertices[1].color    = {0.0f, 1.0f, 0.5f};
	this->vertices[1].texture  = LR_TEX_COORD;

	// 2: Upper Right
	this->vertices[2].position = this->anchor + parallel.UpperRight();
	this->vertices[2].color    = {0.0f, 0.5f, 1.0f};
	this->vertices[2].texture  = UR_TEX_COORD;

	// 3: Upper Left
	this->vertices[3].position = this->anchor + parallel.UpperLeft();
	this->vertices[3].color    = {0.5f, 0.5f, 0.5f};
	this->vertices[3].texture  = UL_TEX_COORD;
}


void Image::OnDraw()
{
	GLsizeiptr data_size = sizeof(this->vertices);

	glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STREAM_DRAW);//GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, this->vertices);

   	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
