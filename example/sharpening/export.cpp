/*
 * export.cpp
 *
 *  Created on: Jan 3, 2020
 *      Author: little
 */
#include "./include/filter.h"
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

const GLchar * fs_tex_filter = STRINGIZE(
	uniform sampler2D Texture;
	uniform uint      Filter;

	in vec4 FragColor;
	in vec2 TexCoord;

	layout (location = 0) out vec4 Color;

	vec4 texel;


	void gaussian()
	{
		ivec2 s   = textureSize(Texture, 0);
		vec2  tmp = TexCoord * vec2(float(s.x), float(s.y));
		ivec2 C   = ivec2(int(tmp.x), int(tmp.y));

		vec4  t00 = texelFetchOffset(Texture, C, 0, ivec2(0, 0));
		vec4  t01 = texelFetchOffset(Texture, C, 0, ivec2(0, 1));
		vec4  t02 = texelFetchOffset(Texture, C, 0, ivec2(0, 0));

		vec4  t10 = texelFetchOffset(Texture, C, 0, ivec2(1, 1));
		vec4  t11 = texelFetchOffset(Texture, C, 0, ivec2(1, 3));
		vec4  t12 = texelFetchOffset(Texture, C, 0, ivec2(1, 1));

		vec4  t20 = texelFetchOffset(Texture, C, 0, ivec2(0, 0));
		vec4  t21 = texelFetchOffset(Texture, C, 0, ivec2(0, 1));
		vec4  t22 = texelFetchOffset(Texture, C, 0, ivec2(0, 0));

		texel = (1.0*t00 + 2.0*t01 + 1.0*t02 + 2.0*t10 + 4.0*t11 + 2.0*t12 + 1.0*t20 + 2.0*t21 + 1.0*t22) * 1.0/16.0;
	}

	void sobel()
	{
		ivec2 s   = textureSize(Texture, 0);
		vec2  tmp = TexCoord * vec2(float(s.x), float(s.y));
		ivec2 C   = ivec2(int(tmp.x), int(tmp.y));

		vec4  t00 = texelFetchOffset(Texture, C, 0, ivec2(-2, -2));
		vec4  t01 = texelFetchOffset(Texture, C, 0, ivec2(-2, -3));
		vec4  t02 = texelFetchOffset(Texture, C, 0, ivec2(-2, -2));

		vec4  t10 = texelFetchOffset(Texture, C, 0, ivec2(-1, -1));
		vec4  t11 = texelFetchOffset(Texture, C, 0, ivec2(-1, -1));
		vec4  t12 = texelFetchOffset(Texture, C, 0, ivec2(-1, -1));

		vec4  t20 = texelFetchOffset(Texture, C, 0, ivec2(0, 0));
		vec4  t21 = texelFetchOffset(Texture, C, 0, ivec2(0, 1));
		vec4  t22 = texelFetchOffset(Texture, C, 0, ivec2(0, 0));

		texel = -1.0*t00 + -2.0*t01 + -1.0*t02 + 0.0*t10 + 0.0*t11 + 0.0*t12 + 1.0*t20 + 2.0*t21 + 1.0*t22;
	}

	void laplace()
	{
		ivec2 s   = textureSize(Texture, 0);
		vec2  tmp = TexCoord * vec2(float(s.x), float(s.y));
		ivec2 C   = ivec2(int(tmp.x), int(tmp.y));

		vec4  t00 = texelFetchOffset(Texture, C, 0, ivec2(-1, -1));
		vec4  t01 = texelFetchOffset(Texture, C, 0, ivec2(-1, 0));
		vec4  t02 = texelFetchOffset(Texture, C, 0, ivec2(-1, -1));

		vec4  t10 = texelFetchOffset(Texture, C, 0, ivec2(0, 0));
		vec4  t11 = texelFetchOffset(Texture, C, 0, ivec2(0, -5));
		vec4  t12 = texelFetchOffset(Texture, C, 0, ivec2(0, 0));

		vec4  t20 = texelFetchOffset(Texture, C, 0, ivec2(-1, -1));
		vec4  t21 = texelFetchOffset(Texture, C, 0, ivec2(-1, 0));
		vec4  t22 = texelFetchOffset(Texture, C, 0, ivec2(-1, -1));

		texel = 0.0*t00 + 1.0*t01 + 0.0*t02 + 1.0*t10 + -4.0*t11 + 1.0*t12 + 0.0*t20 + 1.0*t21 + 0.0*t22;
	}

	void main(void)
	{
		if (1U == Filter)
		{
			gaussian();
		}
		else if (2U == Filter)
		{
			sobel();
		}
		else if (3U == Filter)
		{
			laplace();
		}
		else
		{
			texel = texture(Texture, TexCoord);
		}

		Color = texel;
	}
);
//}} Fragment Shaders
//}} Shaders

using namespace little;



static Raster    header;
static Raster    title;
static Filter    filter;



static const char * str_header = "Adreno GPU SDK";
static const char * str_title  = "Image Filtering";








class InstanceRenderer : public Renderer
{
	uint32_t frame_count;

	GLuint   filter_index;
	GLuint   filter_count;

protected:
	virtual void OnInit();
	virtual void OnDraw();
};



void InstanceRenderer::OnInit()
{
	this->frame_count  = 0;
	this->filter_index = 0;
	this->filter_count = 4;

	const char * imgs[] =
	{
		"/sdcard/DCIM/res/Lenna.png", //RGB //"res/l18.png", //RGB "res/l16.png", //RGB
		"/sdcard/DCIM/res/petal011.png",
		"/sdcard/DCIM/res/petal011.png"
	};

	stbi_set_flip_vertically_on_load(true);

	TextureLoader tex_00(imgs[0]);
	TextureLoader tex_01(imgs[1]);
	TextureLoader tex_02(imgs[2]);

	Raster::Param    param_header   = { str_header, Vector3(0.0f, 0.9f, 0.0f), 0.07f, 0.030f, 1.1f, true  };
	Raster::Param    param_title    = { str_title,  Vector3(0.0f, 0.7f, 0.0f), 0.07f, 0.026f, 0.0f, true  };
	Filter::Param    param_filter   = { "Filter" };

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

	TextureArray ta_header  = ta.SubArray(1, 1);
	TextureArray ta_title   = ta.SubArray(1, 1);
	TextureArray ta_filter  = ta.SubArray(0, 1);

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
		{ true ,  &filter,   vs_common, fs_tex_filter, "MVP", &ta_filter,  &param_filter   },
		{ true ,  &header,   vs_common, fs_frag_tex,   "MVP", &ta_header,  &param_header   },
		{ true ,  &title,    vs_common, fs_frag_tex,   "MVP", &ta_title,   &param_title    }
	};

	this->nodes.Create(ARRAY_LENGTH(na), na);

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

	uint32_t base      = 300;
	uint32_t remainder = this->frame_count % base;

	if (remainder == 0)
	{
		GLuint index = ++this->filter_index % this->filter_count;

		LOG_INFO("FilterIndex", index);

		filter.Select(index);
	}

    this->Renderer::OnDraw();
}



InstanceRenderer renderer;



void InitScene(uint32_t view_width, uint32_t view_height)
{
	renderer.Initialize(view_width, view_height);
}

void DrawImage()
{
	renderer.Draw();
}




