/*
 * ripple.h
 *
 *  Created on: Apr 21, 2020
 *      Author: little
 */

#ifndef INCLUDE_RIPPLE_H_
#define INCLUDE_RIPPLE_H_

#include "gl_render.h"




#include "mesh.h"



namespace little
{



using namespace gl_little;



class Ripple : public Mesh
{
public:
	Ripple(uint32_t width, uint32_t height) :
		Mesh(width, height),
		phase(0),
		selector(0),
		horizontal(true)
	{
		FOOTPRINT();
	}

	struct Algorithm
	{
		float amplitude;
		float omega;
		float scale;

		std::function<float (float, float, const Algorithm & algorithm)> f;

		float delta() {	return T(this->omega) * this->scale; }
	};


	struct Param
	{
		float       scale;
		uint32_t    num;
		Algorithm * algorithms;
	};


public:
	virtual bool Init(const GLchar * vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param);
	virtual void Update(double t, float dt, mat4x4 mv, void * param);

protected:
	virtual void OnDraw();

	virtual void ResetData(void);
	virtual void ColorizeVertex(int x, int y, int pos, bool gradient = true);

public:
	void  Reset(void)                   { this->ResetData(); }

	int   GetSelector(void) const       { return this->selector;         }
	void  SetSelector(int selector)     { this->selector = selector;     }

	bool  GetDirection(void) const      { return this->horizontal;                   }
	void  SetDirection(bool horizontal) { this->horizontal = horizontal;             }
	void  FlipDirection(void)           { this->SetDirection(!this->GetDirection()); }

	uint32_t NumAlgorithms(void) const  { return this->algorithms.Length(); }

	void  Dump(void) const;

protected:
	Array<Vector3>   original_positions;
	Array<Algorithm> algorithms;

	float phase;

	int   selector;
	bool  horizontal;
};


}


#endif /* INCLUDE_RIPPLE_H_ */
