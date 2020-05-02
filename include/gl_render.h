/*
 * gl_render.h
 *
 *  Created on: Jan 10, 2020
 *      Author: little
 */
#ifndef LIB_INCLUDE_GL_RENDER_H_
#define LIB_INCLUDE_GL_RENDER_H_


#include <atomic>




#include "math_tiny.h"
#include "gl_types.h"



#ifndef MULTITHREAD_RENDER
//#define MULTITHREAD_RENDER
#endif  //MULTITHREAD_RENDER

#ifdef MULTITHREAD_RENDER
#include <pthread.h>
#include <semaphore.h>
#endif //MULTITHREAD_RENDER



namespace gl_little
{



class Program
{
public:
	Program() :
		id(0),
		vs_id(0),
		fs_id(0)
	{

	}

	~Program()
	{
		Program::DetachShader(this->id, this->vs_id);
		Program::DetachShader(this->id, this->fs_id);

		if (this->id)
		{
			glDeleteProgram(this->id);
			this->id = 0;
		}
	}

public:
	bool   Link(const GLchar * vs_src, const GLchar * fs_src);

	bool   Init(void)       { return this->id = glCreateProgram(); }
	void   Use(void)  const { glUseProgram(this->id);              }
	GLuint ID(void)   const { return this->id;                     }

public:
	static GLuint AttachShader(GLuint program, GLenum type, const GLchar * src);
	static void   DetachShader(GLuint program, GLuint & shader);

protected:
	GLuint id;
	GLuint vs_id;
	GLuint fs_id;
};



struct Xform
{
	Xform() :
		threshold(360),
		adjusted_threhold(0),
		toggle(1),
		selector(0),
		cycle(0),
		angles{&gamma, &beta, &alpha},
		coefs{1, -1, 1},
		status{true, true, true}
	{
		this->Zoom  = 0.0f;
		this->Delta = 1.0f;

		mat4x4_identity(this->model);
		mat4x4_identity(this->view);
		mat4x4_identity(this->projection);
	}

	void GetModel(mat4x4 m) { this->UpdateModel(); mat4x4_dup(m, this->model); }
	void SetModel(mat4x4 m)	{ mat4x4_dup(this->model, m); }

	void GetView(mat4x4 v)  { mat4x4_dup(v, this->view);  }
	void LookAt(vec3 eye, vec3 center = 0, vec3 up = 0);

	void Perspective(float y_fov, float aspect, float n = 0.1f, float f = 100.0f)
	{
		mat4x4_perspective(this->projection, Radian(y_fov), aspect, n, f);
	}

	void EnableRotations(bool alpha = true, bool beta = true, bool gamma = true)
	{
		this->status[2] = alpha;
		this->status[1] = beta;
		this->status[0] = gamma;
	}

	bool Rotatable(void) const
	{
		return this->status[0] || this->status[1] || this->status[2];
	}

	// Gets ModelView matrix
	void MV(mat4x4 mv)
	{
		this->UpdateModel();
		mat4x4_mul(mv, this->view, this->model);
	}

	// Gets MVP matrix
	void MVP(mat4x4 mvp)
	{
		mat4x4 mv;

		// MV = View * Model
		this->MV(mv);
		// Projection * MV
		mat4x4_mul(mvp, this->projection, mv);
	}

	void Matrices(mat4x4 mv, mat4x4 mvp)
	{
		this->MV(mv);
		this->MVP(mvp);
	}

	void Update(std::function<void (uint32_t)> callback = 0);

	uint32_t GetCycle() const { return this->cycle; }

protected:
	void UpdateModel(void);

public:
	little::Twins<GLfloat> Zoom;
	little::Twins<GLfloat> Delta;

protected:
	mat4x4 model;
	mat4x4 view;
	mat4x4 projection;

	little::Twins<GLfloat> alpha;
	little::Twins<GLfloat> beta;
	little::Twins<GLfloat> gamma;

	GLfloat threshold;
	GLfloat adjusted_threhold;

	int toggle;
	int selector;

	uint32_t cycle;

	static const uint32_t NUM_ANGLE = 3;

	little::Twins<GLfloat> * angles[NUM_ANGLE];
	int	                     coefs [NUM_ANGLE];
	bool                     status[NUM_ANGLE];
};



struct BlendFactor
{
	GLenum src;
	GLenum dst;
};



class Drawable
{
public:
	Drawable() :
		MVP(-1),
		VAO(0),
		VBO(0),
		depth_test(false),
		blending(false)
	{
		FOOTPRINT();

		this->blend_factor = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
	}

	virtual ~Drawable()
	{
		FOOTPRINT();

		glDeleteVertexArrays(1, &this->VAO);
		glDeleteBuffers(1, &this->VBO);
	}

public:
	virtual bool Init(const GLchar * vs, const GLchar* fs, const GLchar * mvp_name, TextureArray * ta, void * param);
	virtual void Update(double t, float dt, mat4x4 mv, void * param) { };
	virtual void Draw(const GLfloat * mvp);

protected:
	virtual void OnDraw() = 0;

public:
	GLint GetUniformLocation(const GLchar  * name);
	void  MvpUniformLocation(const GLchar  * name)  { this->MVP = this->GetUniformLocation(name); }
	void  MvpUniformMatrix  (const GLfloat * mvp);

	void  EnableDepthTest(bool enable) { this->depth_test = enable; }
	void  EnableBlending (bool enable) { this->blending   = enable; }

public:
	operator BlendFactor&() { return this->blend_factor; }

protected:
	void  BindTextures(void);

protected:
	Program program;

	GLint  MVP;
	GLuint VAO;
	GLuint VBO;

	TextureArray textures;

	bool         depth_test;
	bool         blending;
	BlendFactor  blend_factor;
};



class TextureLoader
{
public:
	TextureLoader(const char * filename) :
		image_file(filename)
	{
		FOOTPRINT();

		this->Load();
	}

	~TextureLoader()
	{
		FOOTPRINT();

		this->Cleanup();
	}

public:
	uint8_t  * Texture()        const { return this->data.buffer; }
	uint32_t   Width()          const { return this->data.width;  }
	uint32_t   Height()         const { return this->data.height; }
	int        Components()     const { return this->data.num_components; }

protected:
	void Load(void);
	void Cleanup(void);

protected:
	TextureData   data;
	const char  * image_file;
};



class Renderer
{
public:
	Renderer() :
#ifdef MULTITHREAD_RENDER
		updater_thread(-1),
#endif //MULTITHREAD_RENDER
		view_width(0),
		view_height(0),
		current_time(0),
		delta_time(0),
		last_time(0),
		delta_time_scale(1.0f)
	{
		FOOTPRINT();

		this->last_time = little::GetTime();

		//LOG_INFO("Time:", this->last_time);

#ifdef MULTITHREAD_RENDER
		int status;

		status = sem_init(&this->nodes_sem, 0, 0);
		POSIX_CHECK_ERROR(status);
		status = pthread_mutex_init(&this->nodes_mutex, 0);
		POSIX_CHECK_ERROR(status);
		status = pthread_cond_init(&this->nodes_cond, 0);
		POSIX_CHECK_ERROR(status);
#endif //MULTITHREAD_RENDER
	}

	virtual ~Renderer()
	{
		FOOTPRINT();

#ifdef MULTITHREAD_RENDER
		sem_destroy(&this->nodes_sem);
		pthread_mutex_destroy(&this->nodes_mutex);
		pthread_cond_destroy(&this->nodes_cond);
#endif //MULTITHREAD_RENDER
	}

	struct Node
	{
		bool           enabled;
		Drawable     * drawable;
		const GLchar * vs;
		const GLchar * fs;
		const GLchar * mvp_name;
		TextureArray * textures;
		void         * param;
	};

public:
	void Initialize(uint32_t view_width, uint32_t view_height);
	void Draw();

	float AspectRatio(void)          const;

	float DeltaTimeScale(void)       const { return this->delta_time_scale; }
	void  SetDeltaTimeScale(float v)       { this->delta_time_scale = v;    }

protected:
	virtual void OnInit();
	virtual void OnDraw();

protected:
	void ForEachNode(std::function<void (Node &)> callback);

protected:

#ifdef MULTITHREAD_RENDER
	sem_t           nodes_sem;

	pthread_t       updater_thread;
	pthread_mutex_t nodes_mutex;
	pthread_cond_t  nodes_cond;

	static void * UpdaterThreadFunc(void * obj);
#endif //MULTITHREAD_RENDER

	uint32_t view_width;
	uint32_t view_height;

	std::atomic<double>   current_time;
	std::atomic<float>    delta_time;
	double                last_time;
	float                 delta_time_scale;

	// Transformation
	Xform xform;

	// The objects to draw
	little::Array<Node>  nodes;

	TextureManager texture_manager;
};

inline float Renderer::AspectRatio() const
{
	if (0 == this->view_height || 0 == this->view_width)
		return 1;

	return (float)this->view_width/this->view_height;
}

inline void Renderer::ForEachNode(std::function<void (Node &)> callback)
{
	for (int i = 0; i < this->nodes.Length(); i++)
	{
		Node & node = this->nodes[i];

		if (!node.enabled) continue;

		if (callback) callback(node);
	}
}



} // gl_little




#endif /* LIB_INCLUDE_GL_RENDER_H_ */
