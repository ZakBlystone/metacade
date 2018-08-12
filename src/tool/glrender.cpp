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
//#include "core/core_public.h"
#include "glrender.h"
#include "imgui.h"

#include <iostream>
#include <string>

static const GLchar *g_DefaultVertexSource =
	"#version 150 core\n"
	"in vec2 position;"
	"in vec4 color;"
	"in vec2 texcoord;"
	"out vec4 Color;"
	"out vec2 Texcoord;"
	"uniform mat4 proj;"
	"uniform vec2 off;"
	"void main(void)"
	"{"
	" Color = color;"
	" Texcoord = texcoord;"
	" gl_Position = proj * vec4(position + off, 0.0, 1.0);"
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
static GLuint g_OffsetUniform = 0;

CRendererGL::CRendererGL()
{
	glClearColor(0.35f, 0.35f, 0.35f, 1.0f);

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

		g_OffsetUniform = glGetUniformLocation(g_DefaultShaderProgram, "off");
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

	//std::cout << posAttrib << colAttrib << texAttrib << std::endl;

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

	//Ensure all textures are deleted
	for ( auto entry : _textureRemap )
	{
		ITexture* tex = entry.second;
		if ( tex != nullptr )
		{
			std::cout << "DELETED LINGERING TEXTURE ON GL RENDERER" << std::endl;
			delete tex;
		}
	}
}

void CRendererGL::reshape(int32 width, int32 height)
{
	CMatrix4::orthographic(0.f, 100.f, 0.f, (float)width, 0.f, (float)height, g_Projection);

	glViewport(0, -20, width, height);

	g_ViewportSize[0] = (float)(width);
	g_ViewportSize[1] = (float)(height);

	GLint uniformProjection = glGetUniformLocation(g_DefaultShaderProgram, "proj");
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, g_Projection.get());
}

void CRendererGL::setOffset(CVec2 offset)
{
	glUniform2f(g_OffsetUniform, offset._x, offset._y);
}

void CRendererGL::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CRendererGL::render(class IGameInstance* instance, class IDrawBuffer* buffer)
{
	const IRenderBuffer *renderBuffer = buffer->getRenderBuffer();

	/*std::cout << "Render: " << renderBuffer->getNumVertices() << " vertices, " << renderBuffer->getNumIndices() << " indices, "
		<< buffer->getNumRenderBatches() << " batches" << std::endl;*/
	

	const CVertex2D* vertices = renderBuffer->getVertices();
	const uint16* indices = renderBuffer->getIndices();

	glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
	glBufferData(GL_ARRAY_BUFFER, renderBuffer->getNumVertices() * sizeof(CVertex2D), vertices, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderBuffer->getNumIndices() * sizeof(uint16), indices, GL_STREAM_DRAW);

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

		renderBatch(batch);

		state = batch->getRenderState();
	}

	glBindVertexArray(0);
}

class ITextureProvider* CRendererGL::getTextureProvider()
{
	return this;
}

class CTextureGL : public ITexture
{
public:
	CTextureGL(IImage *imagesource)
	{
		_width = imagesource->getWidth();
		_height = imagesource->getHeight();
		_id = imagesource->getID();
		_refs = 0;

		glGenTextures(1, &_nativeid);
		glBindTexture(GL_TEXTURE_2D, _nativeid);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width,_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagesource->getPixels());

		if ( imagesource->getFlags() & IF_SMOOTH )
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (imagesource->getFlags() & IF_WRAP_X) ? GL_REPEAT : GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (imagesource->getFlags() & IF_WRAP_Y) ? GL_REPEAT : GL_CLAMP);
	}

	virtual ~CTextureGL()
	{
		glDeleteTextures(1, &_nativeid);
	}

	virtual int32 getWidth() const override { return _width; }
	virtual int32 getHeight() const override { return _height; }
	virtual uint32 getID() const override { return _id; }

	void addRef()
	{
		++_refs;
	}

	void decRef()
	{
		--_refs;
	}

	bool isDead()
	{
		return _refs == 0;
	}

	uint32 getNativeID()
	{
		return _nativeid;
	}

private:
	uint32 _id;
	uint32 _nativeid;
	int32 _width;
	int32 _height;
	uint32 _refs;
};

class ITexture* CRendererGL::loadTexture(class IRenderer*, class IImage* imagesource)
{
	auto found = _textureRemap.find(imagesource->getID());
	if ( found != _textureRemap.end() ) 
	{
		CTextureGL* tex = (CTextureGL*) (*found).second;
		tex->addRef();
		return tex;
	}

	CTextureGL* newTexture = new CTextureGL(imagesource);
	newTexture->addRef();

	_textureRemap.insert(std::make_pair(imagesource->getID(), newTexture));

	return newTexture;
}

void CRendererGL::freeTexture(class IRenderer*, ITexture* texture)
{
	if ( texture == nullptr ) return;

	CTextureGL* tex = (CTextureGL*) texture;
	tex->decRef();

	if ( tex->isDead() )
	{
		_textureRemap.erase(_textureRemap.find(tex->getID()));
		delete texture;
	}
}

void CRendererGL::renderBatch(const CRenderBatch* batch)
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
	//std::cout << "State Change: " << stateChangeFlags << std::endl;
	if ( stateChangeFlags & ERenderStateChangeFlags::RSTATECHANGE_BLENDMODE )
	{
		switch ( newState._material._blend )
		{
			case BLEND_NORMAL: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
			case BLEND_ALPHA: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
			case BLEND_ADD: glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
		}		
	}

	if ( stateChangeFlags & ERenderStateChangeFlags::RSTATECHANGE_BASETEXTURE )
	{
		uint32 textureID = newState._material._baseTexture;
		if ( textureID != 0 )
		{
			auto found = _textureRemap.find(textureID);
			if ( found != _textureRemap.end() )
			{
				CTextureGL* tex = (CTextureGL*) (*found).second;
				glBindTexture(GL_TEXTURE_2D, tex->getNativeID());
			}
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, g_WhiteTexture);
		}
	}
}

/*
void CRendererGL::renderGUI(ImDrawData* drawData)
{
	ImGuiIO& io = ImGui::GetIO();
	int fbWidth = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fbHeight = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if ( fbWidth == 0 || fbHeight == 0 ) return;

	drawData->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
    GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
    GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box); 
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);


    // Setup orthographic projection matrix
    glViewport(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight);
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };

	GLint uniformProjection = glGetUniformLocation(g_DefaultShaderProgram, "proj");
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, &ortho_projection[0][0]);
	glBindVertexArray(g_VAO);

	for ( int32 i=0; i<drawData->CmdListsCount; ++i )
	{
		const ImDrawList* cmdList = drawData->CmdLists[i];
		const ImDrawIdx* idxBufferOffset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmdList->VtxBuffer.Size * sizeof(ImDrawVert), (GLvoid*)cmdList->VtxBuffer.Data, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmdList->IdxBuffer.Size * sizeof(ImDrawIdx), (GLvoid*)cmdList->IdxBuffer.Data, GL_STREAM_DRAW);

		for ( int32 j=0; j<cmdList->CmdBuffer.Size; ++j )
		{
			const ImDrawCmd* cmd = &cmdList->CmdBuffer[j];
			if ( cmd->UserCallback )
			{
				cmd->UserCallback(cmdList, cmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)cmd->TextureId);
				glScissor(
					(int32)cmd->ClipRect.x, 
					(int32)(fbHeight - cmd->ClipRect.w), 
					(int32)(cmd->ClipRect.z - cmd->ClipRect.x), 
					(int32)(cmd->ClipRect.w - cmd->ClipRect.y)); 

				glDrawElements(GL_TRIANGLES, (GLsizei)cmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idxBufferOffset);
			}
			idxBufferOffset += cmd->ElemCount;
		}
	}

    // Restore modified GL state
    glUseProgram(last_program);
    glActiveTexture(last_active_texture);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFunc(last_blend_src, last_blend_dst);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}*/