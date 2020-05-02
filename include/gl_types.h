/*
 * gl_types.h
 *
 *  Created on: Nov 21, 2019
 *      Author: little
 */

#ifndef INCLUDE_GL_TYPES_H_
#define INCLUDE_GL_TYPES_H_



#include <cmath>
#include <functional>
#ifdef _OGLES3
#include "OpenGLES/FrmGLES3.h"
//#include <GLES3/gl3.h>
//#include <GLES2/gl2ext.h>
#else
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#endif


#include "linmath.h"
#include "lb_types.h"

#include "../stb/stb_image.h"

#ifndef NDEBUG

#define __GL_LOG(get_iv, get_log, obj, m) \
	do { \
		GLsizei len; \
		get_iv(obj, GL_INFO_LOG_LENGTH, &len); \
		/*LOG_INFO("Length:", len);*/ \
		if (len > 0) \
		{ \
			GLchar *log = new GLchar[len + 1]; \
			get_log(obj, len, &len, log); \
			/*std::cerr << "[ERROR][" << __FILE__ << "@" << __LINE__ << "] " << m << ": " << log << std::endl; */ \
			LOG_ERROR(m, log); \
			delete [] log; \
		} \
	} while(0)

#define SHADER_LOG(_shader, m)		__GL_LOG(glGetShaderiv, glGetShaderInfoLog, _shader, m)

#define PROGRAM_LOG(_program, m)	__GL_LOG(glGetProgramiv, glGetProgramInfoLog, _program, m)

#define GL_CHECK_ERRORS() \
	do { \
		GLenum e; \
		unsigned int finite = 255; \
		while((e = glGetError()) != GL_NO_ERROR && finite--) \
		{ \
			LOG_ERROR("[GL ERROR]", e); \
		} \
	} while(0)

#define GL_CHECK_ERRORS_ONESHOT() \
	do { \
		static int toggle = 1; \
		if (toggle) \
		{ \
			toggle = 0; \
			GL_CHECK_ERRORS(); \
		} \
	} while(0)

#define GL_CHECK_SHADER_CONDITION(_shader, pname, error_condition) \
	do { \
		GLint params; \
		glGetShaderiv(_shader, pname, &params); \
		if (params == error_condition) SHADER_LOG(_shader, ""); \
	} while(0)

#define GL_CHECK_PROGRAM_CONDITION(_program, pname, error_condition) \
	do { \
		GLint params; \
		glGetProgramiv(_program, pname, &params); \
		if (params == error_condition) PROGRAM_LOG(_program, ""); \
	} while(0)

#else //!NDEBUG

#define SHADER_LOG(_shader, m)
#define PROGRAM_LOG(_program, m)
#define GL_CHECK_ERRORS()
#define GL_CHECK_ERRORS_ONESHOT()
#define GL_CHECK_SHADER_CONDITION(_shader, pname, error_condition)
#define GL_CHECK_PROGRAM_CONDITION(_program, pname, error_condition)

#endif //_DEBUG


#define GL_CHECK_CONDITION(condition, msg) \
	do { \
		if (!(condition)) { \
			LOG_ERROR("[GL ERROR]", msg); \
		} \
	} while(0)



#ifndef GL_ES
#define GL_ES
#endif


#ifdef GL_ES
#if 1
#define STRINGIZE(shader)	"#version 310 es\n" "#pragma debug(on)\n" "precision mediump float;\n" #shader
#else
#define STRINGIZE(shader)	"#version 310 es\n" #shader
#endif
#else
#define STRINGIZE(shader)	"#version 330 core\n" #shader
#endif


#ifdef GL_ES
typedef GLushort INDEX_T;
#else  //!GL_ES
typedef GLuint   INDEX_T
#endif //GL_ES



namespace gl_little
{



#ifdef GL_ES
typedef GLushort INDEX_T;
#else  //!GL_ES
typedef GLuint   INDEX_T
#endif //GL_ES



struct Vector3
{
	Vector3() :
		x(0.0f),
		y(0.0f),
		z(0.0f)
	{

	}

	Vector3(GLfloat x, GLfloat y, GLfloat z):
		x(x),
		y(y),
		z(z)
	{

	}

	Vector3(GLfloat x, GLfloat y):
		x(x),
		y(y),
		z(0.0f)
	{

	}

	union {	GLfloat x; GLfloat r; };
	union {	GLfloat y; GLfloat g; };
	union {	GLfloat z; GLfloat b; };

	Vector3 & operator = (const Vector3 & other)
	{
		if (this != &other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
		}

		return *this;
	}

	bool operator == (const Vector3 & other) const
	{
		return this->x == other.x && this->y == other.y && this->z == other.z;
	}

	Vector3 & operator += (const Vector3 & other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;

		return *this;
	}

	Vector3 & operator -= (const Vector3 & other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;

		return *this;
	}

	Vector3 & operator *= (const Vector3 & other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;

		return *this;
	}

	Vector3 & operator *= (float other)
	{
		this->x *= other;
		this->y *= other;
		this->z *= other;

		return *this;
	}

	Vector3 & operator *= (mat4x4 m)
	{
		vec4 new_pos;
		vec4 pos = { this->x, this->y, this->z };

		mat4x4_mul_vec4(new_pos, m, pos);
		*this = Vector3(new_pos[0], new_pos[1], new_pos[2]);

		return *this;
	}

	friend Vector3 operator + (Vector3 left, const Vector3 & right)
	{
		left += right;

		return left;
	}

	friend Vector3 operator - (Vector3 left, const Vector3 & right)
	{
		left -= right;

		return left;
	}

	friend Vector3 operator * (Vector3 left, const Vector3 & right)
	{
		left *= right;

		return left;
	}

	friend Vector3 operator * (Vector3 left, float right)
	{
		left *= right;

		return left;
	}

	friend Vector3 operator * (Vector3 left, mat4x4 right)
	{
		left *= right;

		return left;
	}

	void Dump(bool color = false) const
	{
#ifndef NDEBUG
		if (color)
		{
			std::cout << "R:" << this->r << ", "
					  << "G:" << this->g << ", "
					  << "B:" << this->b << std::endl;
		}
		else
		{
			std::cout << "X:" << this->x << ", "
					  << "Y:" << this->y << ", "
					  << "Z:" << this->z << std::endl;
		}
#endif  //NDEBUG
	}

	void FormatDump(bool color = false) const
	{
#ifndef NDEBUG
		std::cout << "(";
		std::cout << std::fixed << std::setw(9) << std::setprecision(6) << this->x;
		std::cout << ", ";
		std::cout << std::fixed << std::setw(9) << std::setprecision(6) << this->y;
		std::cout << ", ";
		std::cout << std::fixed << std::setw(9) << std::setprecision(6) << this->z;
		std::cout << ")" << std::endl;
#endif //!NDEBUG
	}
};


struct Vector2
{
	Vector2() :
		x(0.0f),
		y(0.0f)
	{

	}

	Vector2(GLfloat x, GLfloat y):
		x(x),
		y(y)
	{

	}

	union {	GLfloat x; GLfloat s; GLfloat w; };
	union {	GLfloat y; GLfloat t; GLfloat h; };

	Vector2 & operator = (const Vector2 & other)
	{
		if (this != &other)
		{
			this->x = other.x;
			this->y = other.y;
		}

		return *this;
	}

	bool operator == (const Vector2 & other) const
	{
		return this->x == other.x && this->y == other.y;
	}

	Vector2 & operator += (const Vector2 & other)
	{
		this->x += other.x;
		this->y += other.y;

		return *this;
	}

	Vector2 & operator -= (const Vector2 & other)
	{
		this->x -= other.x;
		this->y -= other.y;

		return *this;
	}

	Vector2 & operator *= (float other)
	{
		this->x *= other;
		this->y *= other;

		return *this;
	}

	friend Vector2 operator + (Vector2 left, const Vector2 & right)
	{
		left += right;

		return left;
	}

	friend Vector2 operator - (Vector2 left, const Vector2 & right)
	{
		left -= right;

		return left;
	}

	friend Vector2 operator * (Vector2 left, float right)
	{
		left *= right;

		return left;
	}

	void Dump(bool st = false) const
	{
#ifndef NDEBUG
		if (st)
		{
			std::cout << "S:" << this->s << ", "
					  << "T:" << this->t << std::endl;
		}
		else
		{
			std::cout << "X:" << this->x << ", "
					  << "Y:" << this->y << std::endl;
		}
#endif  //NDEBUG
	}
};


struct Vertex
{
	Vector3 position;
	Vector3 color;
	Vector2 texture;

	Vertex & operator = (const Vertex & other)
	{
		if (this != &other)
		{
			this->position = other.position;
			this->color    = other.color;
			this->texture  = other.texture;
		}

		return *this;
	}

	bool operator == (const Vertex & other) const
	{
		return this->position == other.position && this->color == other.color && this->texture == other.texture;
	}

	void Dump() const
	{
#ifndef NDEBUG
		LOG_INFO("Position:", "");
		this->position.Dump(false);
		LOG_INFO("Color:", "");
		this->color.Dump(true);
		LOG_INFO("Texture:", "");
		this->texture.Dump(true);
#endif  //NDEBUG
	}

	static void VertexAttribPointers()
	{
		GLsizeiptr stride = sizeof(Vertex);

		// Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
		// Color
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)sizeof(Vector3));
		// Texture coordinate
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(Vector3) + sizeof(Vector3)));
	}
};



class ParallelView
{
public:
	ParallelView(float width, float height, mat4x4 model_view)
	{
		float hw = width / 2.0f;
		float hh = height / 2.0f;

		vec4   ll_vec_o = {-hw, -hh, 0, 0};
		vec4   lr_vec_o = { hw, -hh, 0, 0};
		vec4   ll_vec, lr_vec;
		mat4x4 inv_mv;

		mat4x4_invert(inv_mv, model_view);
		mat4x4_mul_vec4(ll_vec, inv_mv, ll_vec_o);
		mat4x4_mul_vec4(lr_vec, inv_mv, lr_vec_o);

		this->lower_left  = Vector3(ll_vec[0], ll_vec[1], ll_vec[2]);
		this->lower_right = Vector3(lr_vec[0], lr_vec[1], lr_vec[2]);
		this->upper_left  = this->lower_right * -1;
		this->upper_right = this->lower_left  * -1;
	}

public:
	const Vector3 & LowerLeft(void)  const { return this->lower_left;  }
	const Vector3 & LowerRight(void) const { return this->lower_right; }
	const Vector3 & UpperLeft(void)  const { return this->upper_left;  }
	const Vector3 & UpperRight(void) const { return this->upper_right; }

protected:
	Vector3 lower_left;
	Vector3 lower_right;
	Vector3 upper_left;
	Vector3 upper_right;
};



struct TextureData
{
	uint8_t  * buffer;
	uint32_t   width;
	uint32_t   height;
	int        num_components;
};



/*
 * Aggregate initialization initializes aggregates. It is a form of list-initialization (since C++11) or direct initialization (since C++20)
 * An aggregate is one of the following types:
 * array type
 * class type (typically, struct or union), that has
 *     no private or protected direct (since C++17)non-static data members
 *     no user-declared constructors(until C++11)
 *     no user-provided constructors (explicitly defaulted or deleted constructors are allowed)(since C++11)(until C++17)
 *     no user-provided, inherited, or explicit constructors (explicitly defaulted or deleted constructors are allowed)(since C++17)(until C++20)
 *     no user-declared or inherited constructors(since C++20)
 *     no virtual, private, or protected (since C++17) base classes
 *     no virtual member functions
 *     no default member initializers(since C++11)(until C++14)
 */
struct Texture
{
	TextureData data;
	GLint       internal; // Internal format
	GLenum      format;
	GLenum      type;
	GLuint      handle;
};



class TextureArray : public little::Array<Texture>
{
public:
	TextureArray()
	{

	}

	TextureArray(const little::Array<Texture> & a)
	{
		*this = a;
	}

	TextureArray & operator= (const little::Array<Texture> & a)
	{
		*(little::Array<Texture> *)this = a;

		return *this;
	}

public:
	bool Create(uint32_t length, Texture * init_data) { return little::Array<Texture>::Create(length, init_data); }
};



class TextureManager
{
public:
	TextureManager()
	{

	}

	~TextureManager();

public:
	enum { MAX_TEXTURE_IMAGE_UNITS = 64 };

public:
	operator TextureArray&() { return  this->Textrues(); }
	operator TextureArray*() { return &this->Textrues(); }

public:
	bool Create(uint32_t length, Texture * init_data);

	TextureArray & Textrues(void) { return this->textures; }

protected:
	TextureArray textures;
	GLuint       handles[MAX_TEXTURE_IMAGE_UNITS];
};



extern Vector2 LL_TEX_COORD;
extern Vector2 LR_TEX_COORD;
extern Vector2 UL_TEX_COORD;
extern Vector2 UR_TEX_COORD;



} // namespace gl_little



#ifndef NDEBUG

#define GL_DUMP(view_width, view_height) \
	do { \
		int major = 0; \
		int minor = 0; \
		int context_profile = 0; \
		int n = 0; \
		\
		glGetIntegerv(GL_MAJOR_VERSION, &major); \
		glGetIntegerv(GL_MINOR_VERSION, &minor); \
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &context_profile); \
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n); \
		\
		PRINT_SEPARATOR(); \
		LOG_INFO("Width:", view_width); \
		LOG_INFO("Height:", view_height); \
		LOG_INFO("OpenGLMajorVersion:", major); \
		LOG_INFO("OpenGLMinorVersion:", minor); \
		LOG_INFO("OpenGLVersion:", glGetString(GL_VERSION)); \
		LOG_INFO("OpenGLVendor:", glGetString(GL_VENDOR)); \
		LOG_INFO("OpenGLRendererName:", glGetString(GL_RENDERER)); \
		LOG_INFO("OpenGLContextProfile:", context_profile); \
		LOG_INFO("OpenGLShadingLanguageVersion:", glGetString(GL_SHADING_LANGUAGE_VERSION)); \
		LOG_INFO("GL_MAX_VERTEX_ATTRIBS:", GL_MAX_VERTEX_ATTRIBS); \
		LOG_INFO("MaxVertexAttribs:", n); \
		PRINT_SEPARATOR(); \
	} while(0)

#define M4x4_DUMP(m) \
	do { \
		PRINT_SEPARATOR(); \
		LOG_INFO("Matrix:", #m); \
		for (int i = 0; i < 4; i++) \
		{ \
			for (int j = 0; j < 4; j++) \
				std::cout << m[i][j] << " "; \
			std::cout << std::endl; \
		} \
		PRINT_SEPARATOR(); \
	} while(0)

#define V4_DUMP(v) \
	do { \
		PRINT_SEPARATOR(); \
		LOG_INFO("Vector:", #v); \
		for (int i = 0; i < 4; i++) \
		{ \
			std::cout << v[i] << " "; \
		} \
		std::cout << std::endl; \
		PRINT_SEPARATOR(); \
	} while(0)

#else  //!NDEBUG

#define GL_DUMP(view_width, view_height)
#define M4x4_DUMP(m)
#define V4_DUMP(v)

#endif //NDEBUG



#endif /* INCLUDE_GL_TYPES_H_ */
