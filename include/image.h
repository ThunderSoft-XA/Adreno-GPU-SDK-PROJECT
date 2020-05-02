/*
 * image.h
 *
 *  Created on: Dec 24, 2019
 *      Author: little
 */
#ifndef LIB_INCLUDE_IMAGE_H_
#define LIB_INCLUDE_IMAGE_H_






#include "gl_render.h"



namespace little
{



using namespace gl_little;

class Image : public Drawable
{
public:
	Image()
	{
		FOOTPRINT();

		this->blend_factor.dst = GL_ONE;
	}

	struct Param
	{
		Vector3 anchor;
		Vector2 size;
	};

public:
	virtual bool Init(const GLchar * vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param);
	virtual void Update(double t, float dt, mat4x4 mv, void * param);

protected:
	virtual void OnDraw();

protected:
	Vector3 anchor;
	Vector2 size;

	Vertex  vertices[4];
};






} //namespace little



#endif /* LIB_INCLUDE_IMAGE_H_ */







