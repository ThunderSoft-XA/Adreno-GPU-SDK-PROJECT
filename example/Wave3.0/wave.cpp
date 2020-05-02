/*
 * export.cpp
 *
 *  Created on: Jan 3, 2020
 *      Author: little
 */
#include "./include/image.h"
#include "./include/firework.h"
#include "./include/particle.h"
#include "./include/mesh.h"
#include "./include/raster.h"
#include "./include/wave.h"
#include "./include/lb_common.h"



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

		//texel.g = texel.b = texel.a = texel.r;

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



#define P_TEX_W 8
#define P_TEX_H 8

// Particle texture
uint8_t TextureSharp[P_TEX_W * P_TEX_H] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x11, 0x22, 0x22, 0x11, 0x00, 0x00,
    0x00, 0x11, 0x11, 0x77, 0x77, 0x11, 0x11, 0x00,
    0x00, 0x22, 0x77, 0xFF, 0xEE, 0x77, 0x22, 0x00,
    0x00, 0x22, 0x77, 0xEE, 0xFF, 0x77, 0x22, 0x00,
    0x00, 0x11, 0x11, 0x77, 0x77, 0x11, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x22, 0x22, 0x11, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t TextureSnow[P_TEX_W * P_TEX_H] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x33, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00,
    0x00, 0x00, 0x77, 0x11, 0x77, 0x00, 0x00, 0x00,
    0x33, 0x77, 0x33, 0xFF, 0x33, 0x77, 0x33, 0x00,
    0x00, 0x00, 0x77, 0x11, 0x77, 0x00, 0x00, 0x00,
    0x00, 0x33, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



using namespace little;


#define EXPORT_VERSION 1

#define ENABLE_IMAGE_TEXTURE



//#define USE_DUMMY_VERTICES
#ifdef USE_DUMMY_VERTICES

#include "dummy.h"

little::DummyMesh mesh;

#else  //!USE_DUMMY_VERTICES

#include "../include/mesh.h"

#define GRID_WIDTH	160
#define GRID_HEIGHT	GRID_WIDTH

#if EXPORT_VERSION == 1
little::AnimatedMesh
#else // Not 1
little::FilteringMesh
#endif //EXPORT_VERSION
mesh(GRID_WIDTH, GRID_HEIGHT);

#endif //USE_DUMMY_VERTICES



Image     background;
Raster    blessing;
Raster    title;
Raster    title1;
Raster    title2;

const char * str_header = "Adreno GPU SDK";
const char * str_title  = "Take full advantage of the graphics";
const char * str_title1 = "and computation power";
const char * str_title2 = "provided by the Adreno GPU";

Image::Param     param_float    = { Vector3(0.5f, 0.0f, 0.5f), Vector2(11.6f, 7.6f) };
Mesh::Param      param_floor    = { 1.0f };
Raster::Param    param_header   = { str_header, Vector3(0.0f, 2.1f, 0.0f), 0.14f, 0.11f, 1.1f, true  };
Raster::Param    param_title    = { str_title,  Vector3(0.0f, 1.6f, 0.0f), 0.07f, 0.03f, 0.1f, false };
Raster::Param    param_title1   = { str_title1, Vector3(0.0f, 1.3f, 0.0f), 0.07f, 0.03f, 0.1f, false };
Raster::Param    param_title2   = { str_title2, Vector3(0.0f, 1.0f, 0.0f), 0.07f, 0.03f, 0.1f, false };



class InstanceRenderer : public Renderer
{
protected:
	virtual void OnInit();
	virtual void OnDraw();
};

void InstanceRenderer::OnInit()
{
#ifdef ENABLE_IMAGE_TEXTURE
	LOG_INFO("Define:"," ENABLE_IMAGE_TEXTURE");
	stbi_set_flip_vertically_on_load(true);

	const char * imgs[] =
	{
		"/sdcard/DCIM/res/l16.png",
		"/sdcard/DCIM/res/petal011.png"
	};

	TextureLoader tex_00(imgs[0]);

	param_float.anchor = Vector3(0.4f, 0.0f, -0.1f);
	param_float.size   = Vector2(9.6f, 6.6f);

	TextureLoader tex_01(imgs[1]);

	param_header.particle_size = 0.06f;
#endif //ENABLE_IMAGE_TEXTURE

	Texture tex_ary[] =
	{
#ifdef ENABLE_IMAGE_TEXTURE
		{
			.data     = { .buffer = tex_01.Texture(), .width = tex_01.Width(), .height = tex_01.Height() },
			.internal = GL_RGBA8,
			.format   = GL_RGBA,
			.type     = GL_UNSIGNED_BYTE
		},
#endif //!ENABLE_IMAGE_TEXTURE
		{
			.data     = { .buffer = TextureSharp,     .width = P_TEX_W,        .height = P_TEX_H         },
			.internal = GL_RGB8,
			.format   = GL_RGB,
			.type     = GL_UNSIGNED_BYTE
		}
#ifdef ENABLE_IMAGE_TEXTURE
		,
		{
			.data     = { .buffer = tex_00.Texture(), .width = tex_00.Width(), .height = tex_00.Height() },
			.internal = GL_RGB8,
			.format   = GL_RGB, //GL_RGBA, //
			.type     = GL_UNSIGNED_BYTE
		}
#endif //ENABLE_IMAGE_TEXTURE
	};

	this->texture_manager.Create(ARRAY_LENGTH(tex_ary), tex_ary);

	TextureArray & ta = this->texture_manager.Textrues();

#ifdef ENABLE_IMAGE_TEXTURE
	TextureArray ta_float    = ta.SubArray(2, 1);
	TextureArray ta_header   = ta.SubArray(0, 1);
	TextureArray ta_title    = ta.SubArray(1, 1);
	TextureArray ta_title1   = ta.SubArray(1, 1);
	TextureArray ta_title2   = ta.SubArray(1, 1);
#else  //!ENABLE_IMAGE_TEXTURE
	TextureArray ta_float    = ta.SubArray(0, 1);
	TextureArray ta_header   = ta.SubArray(0, 1);
	TextureArray ta_title    = ta.SubArray(0, 1);
	TextureArray ta_title1   = ta.SubArray(0, 1);
	TextureArray ta_title2   = ta.SubArray(0, 1);
#endif //ENABLE_IMAGE_TEXTURE

	float s = this->AspectRatio();

	if (s < 1.0f)
	{
		param_header.scale         *= s;
		param_header.particle_size *= s;
		param_title.scale          *= (s - 0.1);
		param_title.particle_size  *= s;
		param_title1.scale          = param_title.scale;
		param_title1.particle_size  = param_title.particle_size;
		param_title2.scale          = param_title.scale;
		param_title2.particle_size  = param_title.particle_size;
	}

	((BlendFactor &)title).dst = GL_ONE;
	title.EnableBlending(true);
	((BlendFactor &)title1).dst = GL_ONE;
	title1.EnableBlending(true);
	((BlendFactor &)title2).dst = GL_ONE;
	title2.EnableBlending(true);

	BlendFactor & floor_blend_factor = mesh;
	//floor_blend_factor.dst = GL_ONE;
	//mesh.EnableBlending(true);
	mesh.EnableDepthTest(true);
	mesh.EnableResetData(true);
    mesh.SetMaxFrameNumber(1000);

    mesh.PressureThreshold   = view_width * 0.1;
    mesh.PressurePeriod      = view_width / 50;
    mesh.PressureCoefficient = 0.04;
    mesh.MaxDeltaTime        = 0.02;
    mesh.TimeStepMultiple    = 10;

	Node na[] =
	{
		{ true ,  &background, vs_common, fs_tex,           "MVP", &ta_float,    &param_float    },
		{ true ,  &mesh,       vs_common, fs_frag,          "MVP", 0,            &param_floor    },
#ifdef ENABLE_IMAGE_TEXTURE
		{ true ,  &blessing,   vs_common, fs_frag_tex,      "MVP", &ta_header,   &param_header   },
		{ true ,  &title,      vs_common, fs_frag_luminace, "MVP", &ta_title,    &param_title    },
		{ true ,  &title1,     vs_common, fs_frag_luminace, "MVP", &ta_title1,   &param_title1   },
		{ true ,  &title2,     vs_common, fs_frag_luminace, "MVP", &ta_title2,   &param_title2   },
#else  //!ENABLE_IMAGE_TEXTURE
		{ true ,  &blessing,   vs_common, fs_frag_luminace, "MVP", &ta_header,   &param_header   },
		{ true ,  &title,      vs_common, fs_frag_luminace, "MVP", &ta_title,    &param_title    }
#endif //ENABLE_IMAGE_TEXTURE
	};

	this->nodes.Create(ARRAY_LENGTH(na), na);

    vec3 eye = { 1.5f, 0.0f, 1.5f};

    this->xform.LookAt(eye);
    this->xform.Perspective(90.0f, this->AspectRatio(), 0.1f, 100.0f);
    this->xform.EnableRotations(true, true, true);//(false, false, true);//
    this->xform.Zoom  = -0.3f;
    this->xform.Delta =  0.1f; //1.0f; //0.3f; //

    this->Renderer::OnInit();
}

void InstanceRenderer::OnDraw()
{
	GLenum modes[] = { GL_POINTS, GL_TRIANGLE_STRIP };

    this->xform.Update(
		[&](size_t cycle)
		{
			FOOTPRINT();
			LOG_INFO("Cycle:", cycle);

			if (!this->xform.Rotatable()) return;

			// Change mode in every cycle
			mesh.SetMode(modes[cycle % (ARRAY_LENGTH(modes))]);

			size_t mc = cycle % 4;
			if (1 == mc || 2 == mc)
			{
				this->delta_time_scale = 1.0f;

				LOG_INFO("DeltaTimeScale:", this->delta_time_scale);
			}
			else
			{
				this->delta_time_scale = 0.6f;

				LOG_INFO("DeltaTimeScale:", this->delta_time_scale);
			}

			this->xform.Delta.Reset();
			if ((cycle -1 ) / 4 % 2 == 1)
			{
				this->xform.Delta *= -1;
			}
			this->xform.Delta *= this->delta_time_scale;

			LOG_INFO("Xfrom::Delta:", this->xform.Delta);
		});

    this->Renderer::OnDraw();
}



InstanceRenderer renderer;



void InitScene(uint32_t view_width, uint32_t view_height)
{
	renderer.Initialize(view_width, view_height);
}

void DrawGraphics()
{
	renderer.Draw();
}
