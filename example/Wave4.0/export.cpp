/*
 * export.cpp
 *
 *  Created on: Jan 3, 2020
 *      Author: little
 */
#include "./include/ripple.h"
#include "./include/raster.h"









//{{ Shaders

//{{ Vertex Shaders
const GLchar * vs_common = STRINGIZE(
	uniform mat4 MVP;

	layout (location = 0) in vec4 Position;
	layout (location = 1) in vec4 Color;
	layout (location = 2) in vec2 UV;

	out vec4 FragColor;
	out vec2 TexCoord;

	void main(void)
	{
		FragColor   = Color;
		TexCoord    = UV;
		gl_Position = MVP * Position;
	}
);
//}} Vertex Shaders

//{{ Fragment Shaders
const GLchar * fs_frag_tex = STRINGIZE(
	uniform sampler2D Texture;

	in vec4 FragColor;
	in vec2 TexCoord;

	layout (location = 0) out vec4 Color;

	void main(void)
	{
		vec4 texel = texture(Texture, TexCoord);

		if (texel.a < 0.5)
		{
			discard;
		}
		else
		{
			Color  = texel * FragColor;
		}
	}
);

const GLchar * fs_frag_luminace = STRINGIZE(
	uniform sampler2D Texture;

	in vec4 FragColor;
	in vec2 TexCoord;

	layout (location = 0) out vec4 Color;

	void main(void)
	{
		vec4 texel = texture(Texture, TexCoord);

		texel.g = texel.b = texel.a = texel.r;

		Color  = texel * FragColor;
	}
);

const GLchar * fs_frag_tex_opaque = STRINGIZE(
	uniform sampler2D Texture;

	in vec4 FragColor;
	in vec2 TexCoord;

	layout (location = 0) out vec4 Color;

	void main(void)
	{
		vec4 texel = texture(Texture, TexCoord);

		Color  = texel * FragColor;
	}
);

const GLchar * fs_tex = STRINGIZE(
	uniform sampler2D Texture;

	in vec4 FragColor;
	in vec2 TexCoord;

	layout (location = 0) out vec4 Color;

	void main(void)
	{
		vec4 texel = texture(Texture, TexCoord);

		Color = texel;
	}
);

const GLchar * fs_frag = STRINGIZE(
	uniform sampler2D Texture;

	in vec4 FragColor;
	in vec2 TexCoord;

	layout (location = 0) out vec4 Color;

	void main(void)
	{
		Color = FragColor;
	}
);
//}} Fragment Shaders

//}} Shaders



using namespace little;



#define NUM_GRIDS 200

static Raster    header;
static Raster    title;
static Ripple    ripple(NUM_GRIDS, NUM_GRIDS);



static const char * str_header = "Adreno GPU SDK";
static const char * str_title  = "Wave Effect";







class InstanceRenderer : public Renderer
{
	uint32_t frame_count;

protected:
	virtual void OnInit();
	virtual void OnDraw();
};







void InstanceRenderer::OnInit()
{
	this->frame_count = 0;

	Ripple::Algorithm algorithms[] =
	{
		{
			amplitude: 0.0f,
			omega:     2.0f * M_PI * 1.0f,
			scale:     1.0f,
			f:         [](float x, float phase, const Ripple::Algorithm & algorithm)
			{
				return 0.0f;
			}
		},
		{
			amplitude: 0.01f,
			omega:     2.0f * M_PI * 6.0f,
			scale:     0.5f,
			f:         [](float x, float phase, const Ripple::Algorithm & algorithm)
			{
				return Sine(x, algorithm.omega, phase, algorithm.amplitude, false);
			}
		},
		{
			amplitude: 0.005f,
			omega:     2.0f * M_PI * 3.0f,
			scale:     0.1f,
			f:         [](float x, float phase, const Ripple::Algorithm & algorithm)
			{
				float v =   Sine(x, algorithm.omega,        phase,       algorithm.amplitude,     false) +
							Sine(x, algorithm.omega * 2.0f, phase + 0.1, algorithm.amplitude * 2, false) +
							Sine(x, algorithm.omega * 1.1f, phase + 0.3, algorithm.amplitude * 4, false);
				return v;
			}
		},
		{
			amplitude: 0.005f,
			omega:     2.0f * M_PI * 1.6f,
			scale:     0.1f,
			f:         [](float x, float phase, const Ripple::Algorithm & algorithm)
			{
				x = -x;

				float v =   Sine(x, algorithm.omega,        phase,       algorithm.amplitude,     false) +
							Sine(x, algorithm.omega * 1.1f, phase + 0.1, algorithm.amplitude * 2, false) +
							Sine(x, algorithm.omega * 1.7f, phase + 0.3, algorithm.amplitude * 4, false);
				return v;
			}
		}
	};

	Raster::Param    param_header   = { str_header, Vector3(0.0f, 0.9f, 0.0f), 0.04f, 0.018f, 1.1f, true  };
	Raster::Param    param_title    = { str_title,  Vector3(0.0f, 0.7f, 0.0f), 0.03f, 0.010f, 0.0f, true  };
	Ripple::Param    param_ripple   = { 1.0f, ARRAY_LENGTH(algorithms), algorithms };

	const char * imgs[] =
	{
		"/sdcard/DCIM/res/l18.png", //"res/l16.png",
		"/sdcard/DCIM/res/petal011.png",
		"/sdcard/DCIM/res/petal00.png"
	};

	stbi_set_flip_vertically_on_load(true);

	TextureLoader tex_00(imgs[0]);
	TextureLoader tex_01(imgs[1]);
	TextureLoader tex_02(imgs[2]);

	Texture tex_ary[] =
	{
		{
			.data     = { .buffer = tex_00.Texture(), .width = tex_00.Width(), .height = tex_00.Height() },
			.internal = GL_RGB8,
			.format   = GL_RGB,
			.type     = GL_UNSIGNED_BYTE
		},
		{
			.data     = { .buffer = tex_01.Texture(), .width = tex_01.Width(), .height = tex_01.Height() },
			.internal = GL_RGBA8,
			.format   = GL_RGBA,
			.type     = GL_UNSIGNED_BYTE
		},
		{
			.data     = { .buffer = tex_02.Texture(), .width = tex_02.Width(), .height = tex_02.Height() },
			.internal = GL_RGBA8,
			.format   = GL_RGBA, //GL_RGB, //
			.type     = GL_UNSIGNED_BYTE
		}
	};

	this->texture_manager.Create(ARRAY_LENGTH(tex_ary), tex_ary);

	TextureArray & ta = this->texture_manager.Textrues();

	TextureArray ta_header   = ta.SubArray(1, 1);
	TextureArray ta_title    = ta.SubArray(1, 1);
	TextureArray ta_ripple   = ta.SubArray(0, 1);

	float s = this->AspectRatio();
	if (s < 1.0f)
	{
		param_header.scale         *= s;
		param_header.particle_size *= s;
		param_title.scale          *= (s - 0.1);
		param_title.particle_size  *= s;
	}

	BlendFactor & title_blend_factor = title;
	title_blend_factor.dst = GL_ONE;
	//title.EnableBlending(true);

	BlendFactor & header_blend_factor = header;
	header_blend_factor.dst = GL_ONE;
	//header.EnableBlending(true);

	Node na[] =
	{
		{ true ,  &ripple,   vs_common, fs_tex,      "MVP", &ta_ripple,   &param_ripple   },
		{ true ,  &header,   vs_common, fs_frag_tex, "MVP", &ta_header,   &param_header   },
		{ true ,  &title,    vs_common, fs_frag_tex, "MVP", &ta_title,    &param_title    }
	};

	this->nodes.Create(ARRAY_LENGTH(na), na);

	ripple.SetMode(GL_TRIANGLE_STRIP);

#if 0
    vec3 eye = { 2.2f, 0.0f, 0.2f};

    this->xform.LookAt(eye);
    this->xform.Perspective(90.0f, this->AspectRatio(), 0.1f, 100.0f);
    this->xform.EnableRotations(false, true, true);
    this->xform.Zoom  = -0.3f;
    this->xform.Delta =  0.1f; //0.3f; //1.0f; //
#endif

    this->Renderer::OnInit();
}


void InstanceRenderer::OnDraw()
{
	this->frame_count++;

	uint32_t base = 1000;
	uint32_t noop = 100;

	if (ripple.GetSelector() == 0 && this->frame_count % noop == 0)
	{
		this->frame_count += base - noop;
	}

	uint32_t remainder = this->frame_count % base;

	if (remainder == 0)
	{
		//ripple.Dump();

		uint32_t quotient = this->frame_count / base;
		uint32_t selector = quotient / 2 % ripple.NumAlgorithms();

		ripple.FlipDirection();
		if (selector != ripple.GetSelector())
		{
			ripple.SetSelector(selector);
		}
		ripple.Reset();

		ripple.Dump();
	}

    this->Renderer::OnDraw();
}



InstanceRenderer renderer;



void InitScene(uint32_t view_width, uint32_t view_height)
{
	renderer.Initialize(view_width, view_height);
}

void DrawWave()
{
	renderer.Draw();
}










































