/*
 * filter.cpp
 *
 *  Created on: Jun 1, 2020
 *      Author: little
 */

#include "../include/filter.h"
#include <assert.h>






using namespace little;





bool Filter::Init(const GLchar * vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param)
{
	Filter::Param * p = (Filter::Param *)param;

	assert(param);

	bool status;

	status = Drawable::Init(vs, fs, mvp_name, ta, param);
	if (!status) return false;

	// 0: Lower Left
	this->vertices[0].position = {-1.0f, -1.0f,  0.0f};
	this->vertices[0].color    = { 0.0f,  0.0f,  0.0f};
	this->vertices[0].texture  = LL_TEX_COORD;

	// 1: Lower Right
	this->vertices[1].position = { 1.0f, -1.0f,  0.0f};
	this->vertices[1].color    = { 0.0f,  0.0f,  0.0f};
	this->vertices[1].texture  = LR_TEX_COORD;

	// 2: Upper Right
	this->vertices[2].position = { 1.0f,  1.0f,  0.0f};
	this->vertices[2].color    = { 0.0f,  0.0f,  0.0f};
	this->vertices[2].texture  = UR_TEX_COORD;

	// 3: Upper Left
	this->vertices[3].position = {-1.0f,  1.0f,  0.0f};
	this->vertices[3].color    = { 0.0f,  0.0f,  0.0f};
	this->vertices[3].texture  = UL_TEX_COORD;

	this->filter_location       = this->GetUniformLocation(p->filter_name);

	return true;
}


void Filter::OnDraw(void)
{
	GLsizeiptr data_size = sizeof(this->vertices);

	glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STREAM_DRAW);//GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, this->vertices);

   	glDrawArrays(GL_TRIANGLE_FAN, 0, ARRAY_LENGTH(this->vertices));
}


void Filter::Select(GLuint filter_index)
{
	if (-1 == this->filter_location) return;

	this->program.Use();
	glUniform1ui(this->filter_location, filter_index);
}












