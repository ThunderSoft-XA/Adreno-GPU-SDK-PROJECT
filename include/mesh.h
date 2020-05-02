/*
 * mesh.h
 *
 *  Created on: Oct 31, 2019
 *      Author: little
 */

#ifndef INCLUDE_MESH_H_
#define INCLUDE_MESH_H_

#include "gl_render.h"




using namespace gl_little;



namespace little
{



class Mesh : public Drawable
{
public:
	Mesh(uint32_t width, uint32_t height) :
		width(width),
		height(height),
		scale(1.0f),
		mode(GL_POINTS)
	{
		FOOTPRINT();

		//this->blend_factor.dst = GL_ONE;
	}

	virtual ~Mesh()
	{
		FOOTPRINT();
	}

	struct Param
	{
		float scale;
	};

public:
	virtual bool Init(const GLchar * vs, const GLchar* fs, const GLchar * mvp_name, TextureArray * ta, void * param);

protected:
	virtual void OnDraw();

public:
	uint32_t GetWidth()      const        { return this->width; }
	uint32_t GetHeight()     const        { return this->height; }
	uint32_t VertexPosition(int x, int y) { return y * this->width + x; };

	float  GetScale(void)    const { return this->scale; }
	void   SetScale(float s)       { this->scale = s;    }

	GLenum GetMode(void)     const { return this->mode;  }
	void   SetMode(GLenum m)       { this->mode = m;     }

	Vertex *        GetVertices()       { return this->vertices; }
	const INDEX_T * GetIndexes()  const { return this->indexes;  }

	void Dump() const;
	void DumpIndexes() const;
	void DumpVertex(int pos) const;

public: // Operator
	operator GLsizeiptr() { return this->vertices.Length() * sizeof(Vertex); }

protected:
	void ForEachVertex(std::function <void (int, int)> callback);

protected:
	virtual void ResetData(void);
	virtual void UpdateZvalue(int x, int y, int pos)	{ this->vertices[pos].position.z = 0.f; }
	virtual void ColorizeVertex(int x, int y, int pos, bool gradient = true);
	virtual void ComputeIndex(int x, int y); // For Triangle Strip

	virtual uint32_t NumOfIndexes() const { return 2 * (this->width * (this->height - 1)) + 2 * (this->height - 2); }

protected:
	uint32_t width;
	uint32_t height;

	Array<Vertex>	vertices;
	Array<INDEX_T>	indexes;

	float  scale;
	GLenum mode;
};



class AnimatedMesh : public Mesh
{
public:
	AnimatedMesh(uint32_t width, uint32_t height) :
		Mesh(width, height),
		PressureThreshold(width * 0.1),
		PressurePeriod(width / 50),
		PressureCoefficient(0.01),
		MaxDeltaTime(0.05),
		TimeStepMultiple(10),
		pressure(width, height),
		velocity(width, height),
		last_time(0),
		delta_time(0),
		reset_data(false),
		frame_count(0),
		max_frame_num(-1)
	{
		FOOTPRINT();

		this->last_time = GetTime();
	}

public:
	virtual bool Init(const GLchar * vs, const GLchar* fs, const GLchar * mvp_name, TextureArray * ta, void * param);
	virtual void Update(double t, float dt, mat4x4 mv, void * param);

public:
	void EnableResetData(bool enable) { this->reset_data = enable; }

	uint32_t GetMaxFrameNumber(void)         const { return this->max_frame_num; }
	void     SetMaxFrameNumber(uint32_t num)       { this->max_frame_num = num; }

public:
	struct DualMatrix
	{
		DualMatrix(uint32_t width, uint32_t height) :
			x(width, height),
			y(width, height)
		{
		}

		Matrix<double> x;
		Matrix<double> y;
	};

protected:
	virtual void ResetData(void);
	virtual void InitPressure(void);
	virtual void ComputePressure(void);
	virtual void ApplyPressure(void);

public:
	double PressureThreshold;
	double PressurePeriod;
	double PressureCoefficient;
	double MaxDeltaTime;
	double TimeStepMultiple;

protected:
	Matrix<double> pressure;
	DualMatrix     velocity;

	double last_time;
	double delta_time;

	bool   reset_data;

	uint32_t frame_count;
	uint32_t max_frame_num;
};



class FilteringMesh: public AnimatedMesh
{
public:
	FilteringMesh(uint32_t width, uint32_t height) :
		AnimatedMesh(width, height)
	{
		FOOTPRINT();
	}

protected:
	virtual void UpdateZvalue(int x, int y, int pos);

	virtual void ApplyPressure(void);
};




} // namespace little




#endif /* INCLUDE_MESH_H_ */
