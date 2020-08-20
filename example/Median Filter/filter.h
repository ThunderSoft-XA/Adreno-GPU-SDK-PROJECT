/*
 * filter.h
 *
 *  Created on: Jun 4, 2020
 *      Author: little
 */

#ifndef INCLUDE_FILTER_H_
#define INCLUDE_FILTER_H_

#include "gl_render.h"




#include "mesh.h"



namespace little
{


using namespace gl_little;




class Filter : public Drawable
{
public:
	Filter() :
		filter_location(-1)
	{
		FOOTPRINT();

		this->blend_factor.dst = GL_ONE;
	}

	struct Param
	{
		const GLchar * filter_name;
	};


public:
	virtual bool Init(const GLchar * vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param);



public:
	void Select(GLuint filter_index);



protected:
	virtual void OnDraw();




protected:
	Vertex   vertices[4];

	GLint    filter_location;
};



}





#endif /* INCLUDE_FILTER_H_ */
