/*
Copyright 2017 Zachary Blystone <zakblystone@gmail.com>

This file is part of Metacade.

Metacade is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Metacade is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Metacade.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
===============================================================================
glrender.cpp:
===============================================================================
*/

#include "GL/glew.h"
#include "core/core_public.h"
#include "glrender.h"

static const GLchar *g_DefaultVertexSource =
	"#version 150 core\n"
	"in vec2 position;"
	"in vec4 color;"
	"in vec2 texcoord;"
	"out vec4 Color;"
	"out vec2 Texcoord;"
	"uniform mat4 proj;"
	"void main(void)"
	"{"
	" Color = color;"
	" Texcoord = texcoord;"
	" gl_Position = proj * vec4(position, 0.0, 1.0);"
	"}";

static const GLchar *g_DefaultFragmentSource =
	"#version 150 core\n"
	"in vec4 Color;"
	"in vec2 Texcoord;"
	"out vec4 outColor;"
	"uniform sampler2D tex;"
	"void main(void)"
	"{"
	" outColor = Color * texture(tex, Texcoord);"
	"}";

static CMatrix4 g_Projection = CMatrix4::identity();
static CMatrix4 g_ModelView = CMatrix4::identity();
static CVec2 g_ViewportSize;

static GLuint g_VAO = 0;
static GLuint g_VBO = 0;
static GLuint g_IBO = 0;
static GLuint g_FragmentShader = 0;
static GLuint g_VertexShader = 0;
static GLuint g_DefaultShaderProgram = 0;
static GLuint g_WhiteTexture = 0;

CRendererGL::CRendererGL()
{
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

	glActiveTexture(GL_TEXTURE0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	{
		int32 len;
		char err[4096];

		g_VertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(g_VertexShader, 1, &g_DefaultVertexSource, nullptr);
		glCompileShader(g_VertexShader);

		g_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(g_FragmentShader, 1, &g_DefaultFragmentSource, nullptr);
		glCompileShader(g_FragmentShader);

		glGetShaderInfoLog(g_VertexShader,4096,&len,err); if (err[0]) std::cout << "V: " << std::string(err, len) << std::endl;
		glGetShaderInfoLog(g_FragmentShader,4096,&len,err); if (err[0]) std::cout << "P: " << std::string(err, len) << std::endl;

		g_DefaultShaderProgram = glCreateProgram();
		glAttachShader(g_DefaultShaderProgram, g_VertexShader);
		glAttachShader(g_DefaultShaderProgram, g_FragmentShader);
		glBindFragDataLocation(g_DefaultShaderProgram, 0, "outColor");
		glLinkProgram(g_DefaultShaderProgram);
		glUseProgram(g_DefaultShaderProgram);
	}

	glGenVertexArrays(1, &g_VAO);
	glBindVertexArray(g_VAO);

	glGenBuffers(1, &g_VBO);
	glGenBuffers(1, &g_IBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IBO);

    GLint posAttrib = glGetAttribLocation(g_DefaultShaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(CVertex2D), (void*) offsetof(CVertex2D, _position));

    GLint colAttrib = glGetAttribLocation(g_DefaultShaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(CVertex2D), (void*) offsetof(CVertex2D, _color));

    GLint texAttrib = glGetAttribLocation(g_DefaultShaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(CVertex2D), (void*) offsetof(CVertex2D, _texcoord));
	glBindVertexArray(0);

	std::cout << posAttrib << colAttrib << texAttrib << std::endl;

	//create default white texture
	uint32 whitePixels[16];
	memset(whitePixels, 0xFFFFFFFF, sizeof(whitePixels));

	glGenTextures(1, &g_WhiteTexture);
	glBindTexture(GL_TEXTURE_2D, g_WhiteTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4,4,0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

CRendererGL::~CRendererGL()
{
	glDeleteBuffers(1, &g_VBO);
	glDeleteBuffers(1, &g_IBO);
	glDeleteVertexArrays(1, &g_VAO);
	glDeleteProgram(g_DefaultShaderProgram);
	glDeleteShader(g_VertexShader);
	glDeleteShader(g_FragmentShader);
}

void CRendererGL::reshape(int32 width, int32 height)
{
	CMatrix4::orthographic(0.f, 100.f, 0.f, (float)width, 0.f, (float)height, g_Projection);

	glViewport(0, 0, width, height);

	g_ViewportSize[0] = (float)(width);
	g_ViewportSize[1] = (float)(height);

	GLint uniformProjection = glGetUniformLocation(g_DefaultShaderProgram, "proj");
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, g_Projection.get());
}

void CRendererGL::render(class IDrawBuffer* buffer)
{
	const IRenderBuffer *renderBuffer = buffer->getRenderBuffer();

	std::cout << "Render: " << renderBuffer->getNumVertices() << " vertices, " << renderBuffer->getNumIndices() << " indices, "
		<< buffer->getNumRenderBatches() << " batches" << std::endl;
	

	const CVertex2D* vertices = renderBuffer->getVertices();
	const uint16* indices = renderBuffer->getIndices();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
	glBufferData(GL_ARRAY_BUFFER, renderBuffer->getNumVertices() * sizeof(CVertex2D), vertices, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderBuffer->getNumIndices() * sizeof(uint32), indices, GL_STREAM_DRAW);

	glBindVertexArray(g_VAO);

	//initialize a fresh render state
	CRenderState state = CRenderState();
	updateRenderState(0xFFFFFFFF, state);

	for ( uint32 i=0; i<buffer->getNumRenderBatches(); ++i )
	{
		const CRenderBatch* batch = buffer->getRenderBatches() + i;
		const CRenderState& newState = batch->getRenderState();
		uint32 changeFlags = newState.getStateChangeFlags(state);

		if ( changeFlags != ERenderStateChangeFlags::RSTATECHANGE_NONE )
		{
			updateRenderState(changeFlags, newState);
		}

		renderBatch(buffer, batch);

		state = batch->getRenderState();
	}

	glBindVertexArray(0);
}

ITexture* CRendererGL::loadTexture(class IImage* imagesource)
{
	return nullptr;
}

void CRendererGL::freeTexture(ITexture* texture)
{

}

void CRendererGL::renderBatch(IDrawBuffer* buffer, const CRenderBatch* batch)
{
	switch ( batch->getPrimitive() )
	{
		case PRIM_TRIANGLES:
			glDrawElements(GL_TRIANGLES, batch->getNumIndices(), GL_UNSIGNED_SHORT, (void*)(batch->getFirstIndex() * sizeof(uint16)));
		break;
		case PRIM_LINES:
			glDrawElements(GL_LINES, batch->getNumIndices(), GL_UNSIGNED_SHORT, (void*)(batch->getFirstIndex() * sizeof(uint16)));
		break;
		default:
		break;
	}
}

void CRendererGL::updateRenderState(uint32 stateChangeFlags, const CRenderState& newState)
{
	std::cout << "State Change: " << stateChangeFlags << std::endl;
	if ( stateChangeFlags & ERenderStateChangeFlags::RSTATECHANGE_BLENDMODE )
	{
		switch ( newState._material._blend )
		{
			case BLEND_NORMAL: glBlendFunc(GL_ONE, GL_ZERO); break;
			case BLEND_ALPHA: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
			case BLEND_ADD: glBlendFunc(GL_ONE, GL_ONE); break;
		}		
	}

	if ( stateChangeFlags & ERenderStateChangeFlags::RSTATECHANGE_BASETEXTURE )
	{
		//manage texture switch
	}
}
