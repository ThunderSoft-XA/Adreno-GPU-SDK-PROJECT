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

	void clamp_vec4(void)
	{
		for (int i = 0; i < 4; i++)
		{
			texel[i] = max(0.0, min(1.0, texel[i]));
		}
	}

	void sort2(inout vec4 a0, inout vec4 a1)
	{
		vec4 b0 = min(a0, a1);
		vec4 b1 = max(a0, a1);

		a0 = b0;
		a1 = b1;
	}

	void sort5(inout vec4 a0, inout vec4 a1, inout vec4 a2, inout vec4 a3, inout vec4 a4)
	{
		sort2(a0, a1);
		sort2(a3, a4);
		sort2(a0, a2);
		sort2(a1, a2);
		sort2(a0, a3);
		sort2(a2, a3);
		sort2(a1, a4);
		sort2(a1, a2);
		sort2(a3, a4);
	}

	void median(void)
	{
		vec4 c0 = textureOffset(Texture, TexCoord, ivec2(-1, -1));
		vec4 c1 = textureOffset(Texture, TexCoord, ivec2( 0, -1));
		vec4 c2 = textureOffset(Texture, TexCoord, ivec2( 1, -1));
		vec4 c3 = textureOffset(Texture, TexCoord, ivec2(-1,  0));
		vec4 c4 = textureOffset(Texture, TexCoord, ivec2( 0,  0));
		vec4 c5 = textureOffset(Texture, TexCoord, ivec2( 1,  0));
		vec4 c6 = textureOffset(Texture, TexCoord, ivec2(-1,  1));
		vec4 c7 = textureOffset(Texture, TexCoord, ivec2( 0,  1));
		vec4 c8 = textureOffset(Texture, TexCoord, ivec2( 1,  1));

		sort5(c0, c1, c2, c3, c4);
		sort5(c5, c6, c2, c7, c8);

		texel = c2;
	}

	void mean(void)
	{
		vec4 c0 = textureOffset(Texture, TexCoord, ivec2(-1, -1));
		vec4 c1 = textureOffset(Texture, TexCoord, ivec2( 0, -1));
		vec4 c2 = textureOffset(Texture, TexCoord, ivec2( 1, -1));
		vec4 c3 = textureOffset(Texture, TexCoord, ivec2(-1,  0));
		vec4 c4 = textureOffset(Texture, TexCoord, ivec2( 0,  0));
		vec4 c5 = textureOffset(Texture, TexCoord, ivec2( 1,  0));
		vec4 c6 = textureOffset(Texture, TexCoord, ivec2(-1,  1));
		vec4 c7 = textureOffset(Texture, TexCoord, ivec2( 0,  1));
		vec4 c8 = textureOffset(Texture, TexCoord, ivec2( 1,  1));

		texel = (c0 + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8) * 0.11111;	// 1/9 = 0.11111...
	}

	void mean_weighted(void)
	{
		vec4 c0 = textureOffset(Texture, TexCoord, ivec2(-1, -1));
		vec4 c1 = textureOffset(Texture, TexCoord, ivec2( 0, -1));
		vec4 c2 = textureOffset(Texture, TexCoord, ivec2( 1, -1));
		vec4 c3 = textureOffset(Texture, TexCoord, ivec2(-1,  0));
		vec4 c4 = textureOffset(Texture, TexCoord, ivec2( 0,  0));
		vec4 c5 = textureOffset(Texture, TexCoord, ivec2( 1,  0));
		vec4 c6 = textureOffset(Texture, TexCoord, ivec2(-1,  1));
		vec4 c7 = textureOffset(Texture, TexCoord, ivec2( 0,  1));
		vec4 c8 = textureOffset(Texture, TexCoord, ivec2( 1,  1));

		texel = (c0 + c1 + c2 + c3 + c5 + c6 + c7 + c8) * 0.125;	// 1/8 = 0.125
		texel = texel * 0.5 + c4 * 0.5;

		clamp_vec4();
	}

	void main(void)
	{
		switch(Filter)
		{
		case 1U:
			median();
			break;
		case 2U:
			mean();
			break;
		case 3U:
			mean_weighted();
			break;
		default:
			texel = texture(Texture, TexCoord);
			break;
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
		"/sdcard/DCIM/res/Lenna_Salt.png", //"res/Lenna.png", //RGB //"res/l18.png", //RGB "res/l16.png", //RGB
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

void DrawFilter()
{
	renderer.Draw();
}




