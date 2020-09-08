/*
 * export.cpp
 *
 *  Created on: Jan 3, 2020
 *      Author: little
 */
#include "../include/filter.h"
#include "../include/raster.h"








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

	void clamp_vec4(inout vec4 texel)
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

	void f_3x3(in float coefs[9], inout vec4 texel)
	{
		vec4 c00 = textureOffset(Texture, TexCoord, ivec2(-1, -1));
		vec4 c01 = textureOffset(Texture, TexCoord, ivec2( 0, -1));
		vec4 c02 = textureOffset(Texture, TexCoord, ivec2( 1, -1));

		vec4 c10 = textureOffset(Texture, TexCoord, ivec2(-1,  0));
		vec4 c11 = textureOffset(Texture, TexCoord, ivec2( 0,  0));
		vec4 c12 = textureOffset(Texture, TexCoord, ivec2( 1,  0));

		vec4 c20 = textureOffset(Texture, TexCoord, ivec2(-1,  1));
		vec4 c21 = textureOffset(Texture, TexCoord, ivec2( 0,  1));
		vec4 c22 = textureOffset(Texture, TexCoord, ivec2( 1,  1));

		texel = c00 * coefs[0] + c01 * coefs[1] + c02 * coefs[2] +
				c10 * coefs[3] + c11 * coefs[4] + c12 * coefs[5] +
				c20 * coefs[6] + c21 * coefs[7] + c22 * coefs[8];
	}

	void f_7x7(in float coefs[49], inout vec4 texel)
	{
		vec4 c00 = textureOffset(Texture, TexCoord, ivec2(-3, -3));
		vec4 c01 = textureOffset(Texture, TexCoord, ivec2(-2, -3));
		vec4 c02 = textureOffset(Texture, TexCoord, ivec2(-1, -3));
		vec4 c03 = textureOffset(Texture, TexCoord, ivec2( 0, -3));
		vec4 c04 = textureOffset(Texture, TexCoord, ivec2( 1, -3));
		vec4 c05 = textureOffset(Texture, TexCoord, ivec2( 2, -3));
		vec4 c06 = textureOffset(Texture, TexCoord, ivec2( 3, -3));

		vec4 c10 = textureOffset(Texture, TexCoord, ivec2(-3, -2));
		vec4 c11 = textureOffset(Texture, TexCoord, ivec2(-2, -2));
		vec4 c12 = textureOffset(Texture, TexCoord, ivec2(-1, -2));
		vec4 c13 = textureOffset(Texture, TexCoord, ivec2( 0, -2));
		vec4 c14 = textureOffset(Texture, TexCoord, ivec2( 1, -2));
		vec4 c15 = textureOffset(Texture, TexCoord, ivec2( 2, -2));
		vec4 c16 = textureOffset(Texture, TexCoord, ivec2( 3, -2));

		vec4 c20 = textureOffset(Texture, TexCoord, ivec2(-3, -1));
		vec4 c21 = textureOffset(Texture, TexCoord, ivec2(-2, -1));
		vec4 c22 = textureOffset(Texture, TexCoord, ivec2(-1, -1));
		vec4 c23 = textureOffset(Texture, TexCoord, ivec2( 0, -1));
		vec4 c24 = textureOffset(Texture, TexCoord, ivec2( 1, -1));
		vec4 c25 = textureOffset(Texture, TexCoord, ivec2( 2, -1));
		vec4 c26 = textureOffset(Texture, TexCoord, ivec2( 3, -1));

		vec4 c30 = textureOffset(Texture, TexCoord, ivec2(-3,  0));
		vec4 c31 = textureOffset(Texture, TexCoord, ivec2(-2,  0));
		vec4 c32 = textureOffset(Texture, TexCoord, ivec2(-1,  0));
		vec4 c33 = textureOffset(Texture, TexCoord, ivec2( 0,  0));
		vec4 c34 = textureOffset(Texture, TexCoord, ivec2( 1,  0));
		vec4 c35 = textureOffset(Texture, TexCoord, ivec2( 2,  0));
		vec4 c36 = textureOffset(Texture, TexCoord, ivec2( 3,  0));

		vec4 c40 = textureOffset(Texture, TexCoord, ivec2(-3,  1));
		vec4 c41 = textureOffset(Texture, TexCoord, ivec2(-2,  1));
		vec4 c42 = textureOffset(Texture, TexCoord, ivec2(-1,  1));
		vec4 c43 = textureOffset(Texture, TexCoord, ivec2( 0,  1));
		vec4 c44 = textureOffset(Texture, TexCoord, ivec2( 1,  1));
		vec4 c45 = textureOffset(Texture, TexCoord, ivec2( 2,  1));
		vec4 c46 = textureOffset(Texture, TexCoord, ivec2( 3,  1));

		vec4 c50 = textureOffset(Texture, TexCoord, ivec2(-3,  2));
		vec4 c51 = textureOffset(Texture, TexCoord, ivec2(-2,  2));
		vec4 c52 = textureOffset(Texture, TexCoord, ivec2(-1,  2));
		vec4 c53 = textureOffset(Texture, TexCoord, ivec2( 0,  2));
		vec4 c54 = textureOffset(Texture, TexCoord, ivec2( 1,  2));
		vec4 c55 = textureOffset(Texture, TexCoord, ivec2( 2,  2));
		vec4 c56 = textureOffset(Texture, TexCoord, ivec2( 3,  2));

		vec4 c60 = textureOffset(Texture, TexCoord, ivec2(-3,  3));
		vec4 c61 = textureOffset(Texture, TexCoord, ivec2(-2,  3));
		vec4 c62 = textureOffset(Texture, TexCoord, ivec2(-1,  3));
		vec4 c63 = textureOffset(Texture, TexCoord, ivec2( 0,  3));
		vec4 c64 = textureOffset(Texture, TexCoord, ivec2( 1,  3));
		vec4 c65 = textureOffset(Texture, TexCoord, ivec2( 2,  3));
		vec4 c66 = textureOffset(Texture, TexCoord, ivec2( 3,  3));

		texel = c00 * coefs[ 0] + c01 * coefs[ 1] + c02 * coefs[ 2] + c03 * coefs[ 3] + c04 * coefs[ 4] + c05 * coefs[ 5] + c06 * coefs[ 6] +
				c10 * coefs[ 7] + c11 * coefs[ 8] + c12 * coefs[ 9] + c13 * coefs[10] + c14 * coefs[11] + c15 * coefs[12] + c16 * coefs[13] +
				c20 * coefs[14] + c21 * coefs[15] + c22 * coefs[16] + c23 * coefs[17] + c24 * coefs[18] + c25 * coefs[19] + c26 * coefs[20] +
				c30 * coefs[21] + c31 * coefs[22] + c32 * coefs[23] + c33 * coefs[24] + c34 * coefs[25] + c35 * coefs[26] + c36 * coefs[27] +
				c40 * coefs[28] + c41 * coefs[29] + c42 * coefs[30] + c43 * coefs[31] + c44 * coefs[32] + c45 * coefs[33] + c46 * coefs[34] +
				c50 * coefs[35] + c51 * coefs[36] + c52 * coefs[37] + c53 * coefs[38] + c54 * coefs[39] + c55 * coefs[40] + c56 * coefs[41] +
				c60 * coefs[42] + c61 * coefs[43] + c62 * coefs[44] + c63 * coefs[45] + c64 * coefs[46] + c65 * coefs[47] + c66 * coefs[48];
	}

	void a9_scale(inout float a[9], float v)
	{
		for (int i = 0; i < 9; i++)
		{
			a[i] = a[i] * v;
		}
	}

	const highp vec3 W = vec3(0.33, 0.33, 0.33); //  vec3(0.0, 1.0, 0.0); //

	vec4 gray(void)
	{
		float coefs[9] = float[](
				 1.0,  1.0,  1.0,
				 1.0,  1.0,  1.0,
				 1.0,  1.0,  1.0
				);

		vec4 texel;

		a9_scale(coefs, 0.111);
		f_3x3(coefs, texel);

		float luminance = dot(texel.rgb, W);

		return vec4(vec3(luminance), 1.0);
	}

	const vec4 bg_color = vec4(0.5, 0.5, 0.5, 1.0);

	vec4 emboss(void)
	{
		vec4 texel = texture(Texture, TexCoord);
		vec4 up_left_texel = textureOffset(Texture, TexCoord, ivec2(-1, -1));
		vec4 delta = texel - up_left_texel;

		float luminance = dot(delta.rgb, W);

		texel = vec4(vec3(luminance), 0.0) + bg_color;

		return texel;
	}

	void main(void)
	{
		vec4 texel;

		switch(Filter)
		{
		case 1U:
			texel = gray();
			break;
		case 2U:
			texel = emboss();
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
	this->filter_count = 3;

	const char * imgs[] =
	{
		"sdcard/DCIM/res/Lenna.png", //"res/Lenna_Salt.png", //RGB //"res/l18.png", //RGB "res/l16.png", //RGB
		"sdcard/DCIM/res/petal011.png",
		"sdcard/DCIM/res/petal011.png"
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




