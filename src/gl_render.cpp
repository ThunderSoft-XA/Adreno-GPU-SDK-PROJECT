/*
 * gl_render.cpp
 *
 *  Created on: Nov 21, 2019
 *      Author: little
 */
#include <sys/time.h>
#include <assert.h>


#include "../include/gl_render.h"




using namespace little;




namespace gl_little
{



GLuint Program::AttachShader(GLuint program, GLenum type, const GLchar * src)
{
	GL_CHECK_CONDITION(src != 0, "Shader source is missing!");

    auto attach_shader = [](GLuint program, GLenum shader_type, const GLchar **shader_source)
	{
		GLuint shader = glCreateShader(shader_type);

		glShaderSource(shader, 1, shader_source, 0);
		glCompileShader(shader);
		GL_CHECK_SHADER_CONDITION(shader, GL_COMPILE_STATUS, GL_FALSE);
		glAttachShader(program, shader);

		return shader;
	};

    return attach_shader(program, type, &src);
}

void Program::DetachShader(GLuint program, GLuint & shader)
{
	if (program && shader)
	{
		glDetachShader(program, shader);
		glDeleteShader(shader);
		shader = 0;
	}
}

bool Program::Link(const GLchar * vs_src, const GLchar * fs_src)
{
	bool status;

	if (!this->id)
	{
		status = this->Init();

		if (!status) return false;
	}

	GL_CHECK_CONDITION(this->id != 0, "Program doesn't initialized!");

	if (vs_src)
		this->vs_id = Program::AttachShader(this->id, GL_VERTEX_SHADER, vs_src);
	if (fs_src)
		this->fs_id = Program::AttachShader(this->id, GL_FRAGMENT_SHADER, fs_src);

    glLinkProgram(this->id);

    GL_CHECK_PROGRAM_CONDITION(this->id, GL_LINK_STATUS, GL_FALSE);

    return true;
}



void Xform::LookAt(vec3 eye, vec3 center, vec3 up)
{
	if (!eye)
	{
		static vec3 _eye = {1.0f, 1.0f, 1.0f};
		eye = _eye;
	};
	if (!center)
	{
		static vec3 _center = {0.0f, 0.0f, 0.0f};
		center = _center;
	};
	if (!up)
	{
		static vec3 _up = {0.0f, 0.0f, 1.0f};
		up = _up;
	};

	mat4x4_look_at(this->view, eye, center, up);
}

void Xform::Update(std::function<void (uint32_t)> callback)
{
	if (callback && this->selector % NUM_ANGLE == 0 && 1 == this->toggle)
	{
		this->cycle++;

		callback(this->cycle);
	}

	auto inc_selector = [&]()
	{
		this->selector = (this->selector + 1) % NUM_ANGLE;
	};

	// Disable rotation
	if (!status[this->selector])
	{
		inc_selector();
		this->toggle = 1;

		//LOG_INFO("Selector:", this->selector);

		return;
	}

	Twins<GLfloat> & angle = *angles[selector];

	if (1 == this->toggle)
	{
		angle.Reset();
		this->adjusted_threhold = angle + coefs[selector] * this->threshold;
		this->toggle = 0;

		LOG_INFO("Selector:", this->selector);
		LOG_INFO("Threshold:", this->threshold);
	}

	angle += coefs[selector] * this->Delta;

	if (std::abs(angle) >= std::abs(this->adjusted_threhold))
	{
		inc_selector();
		this->toggle = 1;

		LOG_INFO("Selector:", this->selector);
	}
}

void Xform::UpdateModel()
{
	mat4x4_identity(this->model);
	mat4x4_translate(this->model, 0.0f, 0.0f, this->Zoom);
	mat4x4_rotate_X(this->model, this->model, Radian(this->alpha));
	mat4x4_rotate_Y(this->model, this->model, Radian(this->beta));
	mat4x4_rotate_Z(this->model, this->model, Radian(this->gamma));
}



TextureManager::~TextureManager()
{
	FOOTPRINT();

	glDeleteTextures(this->textures.Length(), this->handles);
	GL_CHECK_CONDITED(GL_NO_ERROR == glGetError(), "GL_GETERROR");
}

bool TextureManager::Create(uint32_t length, Texture * init_data)
{
	if (0 == length || !init_data)
	{
		LOG_ERROR("TextureManager::Create():", "Invalid parameters!");

		return false;
	}

	if (length > MAX_TEXTURE_IMAGE_UNITS)
	{
		LOG_ERROR("TextureImageUnits:", MAX_TEXTURE_IMAGE_UNITS);
		LOG_ERROR("GivenLength:", length);

		return false;
	}

	if (!this->textures.Create(length, init_data)) return false;

	assert(this->textures.Length() == length);

	PRINT_SEPARATOR();
	FOOTPRINT();

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glGenTextures(length, (GLuint *)this->handles);

	for (uint32_t i = 0; i < length; i++)
	{
		//LOG_INFO("TextureIndex:", i);
		//LOG_INFO("TextureHandle:", this->handles[i]);

		this->textures[i].handle = this->handles[i];

		/*
		 * DO NOT call glIsTexture() before calling glBindTexture()
		 */
		glBindTexture(GL_TEXTURE_2D, this->textures[i].handle);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexImage2D(
	    		GL_TEXTURE_2D,
	    		0,
				this->textures[i].internal,
				this->textures[i].data.width,
				this->textures[i].data.height,
				0,
				this->textures[i].format,
				this->textures[i].type,
				this->textures[i].data.buffer);

		GLboolean is_texture = glIsTexture(this->handles[i]);

		if (!is_texture)
		{
			LOG_ERROR("InvalidTexture:", this->handles[i]);

			continue;
		}
		GL_CHECK_CONDITED(GL_NO_ERROR == glGetError(), "GL_GETERROR");
		//GL_CHECK_ERRORS();

	}
	GL_CHECK_CONDITED(GL_NO_ERROR == glGetError(), "GL_GETERROR");
	PRINT_SEPARATOR();

	return true;
}



bool Drawable::Init(const GLchar * vs, const GLchar * fs, const GLchar * mvp_name, TextureArray * ta, void * param)
{
	bool status;

	assert(mvp_name);

    status = this->program.Init();
    if (!status) return false;

	status = this->program.Link(vs, fs);
	if (!status) return false;

	this->MvpUniformLocation(mvp_name);
	if (ta) this->textures = *ta;

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	this->BindTextures();
	GL_CHECK_CONDITED(GL_NO_ERROR == glGetError(), "GL_GETERROR");

	return this->VAO && this->VBO;
}

void Drawable::Draw(const GLfloat * mvp)
{
	this->MvpUniformMatrix(mvp);
	this->BindTextures();

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	Vertex::VertexAttribPointers();

	if (this->blending)
	{
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(this->blend_factor.src, this->blend_factor.dst);
	}
	else
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	if (this->depth_test)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	/**
	 * glEnable/glDisable(GL_TEXTURE_2D) is deprecated from OpenGL 3+
	if (this->textures.Length() > 0)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
	*/

	this->OnDraw();

	/*
	 * glEnable/glDisable(GL_TEXTURE_2D) is deprecated from OpenGL 3+
	if (this->textures.Length() > 0)
	{
		glDisable(GL_TEXTURE_2D);
	}
	*/

	if (this->depth_test)
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (this->blending)
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
	GL_CHECK_CONDITED(GL_NO_ERROR == glGetError(), "GL_GETERROR");
}

GLint Drawable::GetUniformLocation(const GLchar * name)
{
	GLuint id = this->program.ID();

	this->program.Use();
	this->MVP = glGetUniformLocation(id, name);

	PROGRAM_LOG(id, "GetUniformLocation");

	PRINT_SEPARATOR();
	LOG_INFO("Program:", id);
	LOG_INFO("UniformName:", name);
	LOG_INFO("Location:", this->MVP);
	PRINT_SEPARATOR();

	return this->MVP;
}

void Drawable::MvpUniformMatrix(const GLfloat * mvp)
{
	this->program.Use();
	glUniformMatrix4fv(this->MVP, 1, GL_FALSE, (const GLfloat *)mvp);
}

void Drawable::BindTextures()
{
	uint32_t len = this->textures.Length();

	for (uint32_t i = 0; i < len; i++)
	{
		GLenum tex = GL_TEXTURE0 + i;

		glActiveTexture(tex);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].handle);
	}
}



void TextureLoader::Load()
{
	this->data.buffer = stbi_load(this->image_file, (int *)&this->data.width, (int *)&this->data.height, (int *)&this->data.num_components, 0);

	if (!this->data.buffer)
	{
		LOG_ERROR("Image load failed:", image_file);

		return;
	}

	PRINT_SEPARATOR();
	LOG_INFO("Image:", this->image_file);
	LOG_INFO("Texture:", (void *)this->data.buffer);
	LOG_INFO("TextureWidth:", this->Width());
	LOG_INFO("TextureHeight:", this->Height());
	LOG_INFO("Components:", this->data.num_components);
	PRINT_SEPARATOR();
}

void TextureLoader::Cleanup()
{
	if (this->data.buffer)
	{
		stbi_image_free(this->data.buffer);

		this->data.buffer = 0;
	}

	this->data.width          = 0;
	this->data.height         = 0;
	this->data.num_components = 0;
}



#ifndef NDEBUG

static GLenum IgnoredTypes[] =
{
	GL_DEBUG_TYPE_OTHER
};

static void GLAPIENTRY MessageCallback(
		GLenum  source,
		GLenum  type,
		GLuint  id,
		GLenum  severity,
		GLsizei length,
		const GLchar * message,
		const void   * userParam)
{
	for (int i = 0; i < ARRAY_LENGTH(IgnoredTypes); i++)
	{
		if (type == IgnoredTypes[i]) return;
	}

	PRINT_SEPARATOR();
	FOOTPRINT();
	if (GL_DEBUG_TYPE_ERROR == type)
	{
		LOG_ERROR("DebugType:", "GL_DEBUG_TYPE_ERROR");
	}
	else
	{
		LOG_INFO("DebugType:", type);
	}
	LOG_INFO("Source:", source);
	LOG_INFO("ID:", id);
	LOG_INFO("Severity:", severity);
	LOG_INFO("Message:", message);
	PRINT_SEPARATOR();
}

#endif //NDEBUG

void Renderer::Initialize(uint32_t view_width, uint32_t view_height)
{
	this->view_width  = view_width;
	this->view_height = view_height;

	GL_DUMP(this->view_width, this->view_height);

	struct timeval tv;

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec);

#ifndef NDEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif //NDEBUG

	glDisable(GL_CULL_FACE);
	// The default front face is GL_CCW
	//glFrontFace(GL_CW); //GL_CCW); //

	this->OnInit();
	GL_CHECK_CONDITED(GL_NO_ERROR == glGetError(), "GL_GETERROR");

	GL_CHECK_ERRORS();
}

void Renderer::Draw()
{
	this->current_time = GetTime();
	this->delta_time   = (this->current_time - this->last_time) * this->delta_time_scale;
	this->last_time    = this->current_time;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glViewport(0, 0, this->view_width, this->view_height);

	this->OnDraw();
	GL_CHECK_CONDITED(GL_NO_ERROR == glGetError(), "GL_GETERROR");

	GL_CHECK_ERRORS_ONESHOT();
}

#ifdef MULTITHREAD_RENDER
void * Renderer::UpdaterThreadFunc(void * obj)
{
	Renderer * renderer = static_cast<Renderer *>(obj);
	int        sem_val  = 0;

	assert(renderer);

	while(1)
	{
		sem_getvalue(&renderer->nodes_sem, &sem_val);
		assert(sem_val <= 1);

	    // Wait the semaphore to be ready, the initial value of the semaphore is 0
	    sem_wait(&renderer->nodes_sem);
		//FOOTPRINT();

	    mat4x4 mv, mvp;

	    renderer->xform.Matrices(mv, mvp);

	    pthread_mutex_lock(&renderer->nodes_mutex);

		renderer->ForEachNode([&](Node & node)
			{
				node.drawable->Update(renderer->current_time, renderer->delta_time, mv, node.param);
			}
		);

		pthread_cond_signal(&renderer->nodes_cond);
		pthread_mutex_unlock(&renderer->nodes_mutex);
	}

    return (void *)0;
}
#endif //MULTITHREAD_RENDER

void Renderer::OnInit()
{
	for (int i = 0; i < this->nodes.Length(); i++)
	{
		Node & node = this->nodes[i];

		if (!node.enabled) continue;

		node.drawable->Init(node.vs, node.fs, node.mvp_name, node.textures, node.param);
	}

#ifdef MULTITHREAD_RENDER

	int status = pthread_create(&this->updater_thread, 0, Renderer::UpdaterThreadFunc, (void *)this);

#endif //MULTITHREAD_RENDER
}

void Renderer::OnDraw()
{
    mat4x4 mv, mvp;

    this->xform.Matrices(mv, mvp);

#ifdef MULTITHREAD_RENDER
    //FOOTPRINT();

	int sem_val = 0;

	pthread_mutex_lock(&this->nodes_mutex);

	for (sem_getvalue(&this->nodes_sem, &sem_val); sem_val > 0; sem_getvalue(&this->nodes_sem, &sem_val))
	{
		/**
		 * pthread_cond_wait() atomically release mutex and cause the calling thread to block on the condition variable
		 */
		pthread_cond_wait(&this->nodes_cond, &this->nodes_mutex);
	}

	// Call Drawable::Draw() only if semaphore value is equal to 1
	this->ForEachNode([&](Node & node)
		{
			node.drawable->Draw((const GLfloat *)mvp);
		}
	);

	pthread_mutex_unlock(&this->nodes_mutex);

    // Make sure the maximum value of semaphore is 1
    sem_getvalue(&this->nodes_sem, &sem_val);
    if (sem_val == 0)
    {
    	// Increments the semaphore only if its value is equal to 0
    	sem_post(&this->nodes_sem);
    }
#else  //!MULTITHREAD_RENDER
	this->ForEachNode([&](Node & node)
		{
			node.drawable->Draw((const GLfloat *)mvp);
		}
	);
	this->ForEachNode([&](Node & node)
		{
			node.drawable->Update(this->current_time, this->delta_time, mv, node.param);
		}
	);
#endif //MULTITHREAD_RENDER
}



Vector2 LL_TEX_COORD(0.0f, 0.0f);
Vector2 LR_TEX_COORD(1.0f, 0.0f);
Vector2 UL_TEX_COORD(0.0f, 1.0f);
Vector2 UR_TEX_COORD(1.0f, 1.0f);



} // namespace gl_little

