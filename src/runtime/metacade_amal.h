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
metacade_amal.h: amalgamated header for metacade
===============================================================================
*/
#pragma once
//src/runtime/metacade_public.h
//src/runtime/metacade_types.h
#ifdef WINDOWS
#if defined(METACADE_API_EXPORT)
#   define METACADE_API   __declspec(dllexport)
#else // outside DLL
#   define METACADE_API   __declspec(dllimport)
#endif
#else //Linux
	#define METACADE_API
#endif
#ifndef ARCADE_CTYPES_DEFINED
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
#endif
namespace Arcade
{
enum EFileIOMode
{
	FILE_READ,
	FILE_WRITE,
};
enum EPointClassify
{
	PLANE_BEHIND = -1,
	PLANE_ON = 0,
	PLANE_INTERSECT = 0,
	PLANE_INFRONT = 1,
};
enum ELanguage
{
	LANG_DUMMY,
	LANG_LUA,
	//LANG_PYTHON,
};
#define MAX_BLEND_BITS 3
enum EBlendMode
{
	BLEND_NORMAL = 0,
	BLEND_ADD = 1,
	BLEND_ALPHA = 2,
};
enum EPrimitive
{
	PRIM_TRIANGLES,
	PRIM_LINES,
};
enum EImagePixelFormat
{
	PFM_ALPHA8,
	PFM_RGB8,
	PFM_RGBA8,
	PFM_BGR8,
	PFM_BGRA8,
	PFM_LUMINANCE8,
};
enum ERenderElementType
{
	RET_NONE,
	RET_QUAD,
};
enum ERenderStateChangeFlags
{
	RSTATECHANGE_NONE = 0x0,
	RSTATECHANGE_BLENDMODE = 0x1,
	RSTATECHANGE_BASETEXTURE = 0x2,
};
enum EMessageType
{
	LOG_MESSAGE,
	LOG_WARN,
	LOG_ERROR,
	LOG_MAX,
};
}
//src/runtime/core/core_public.h
//src/runtime/core/public/math/matrix3.h
namespace Arcade
{
class CVec2;
class METACADE_API CMatrix3 
{
public:
	CMatrix3();
	CMatrix3(float m[9]) {set(m);}
	void set(float m[9]) {for(int i=0; i<9; i++) m_[i] = m[i];} //Temporary (use memcpy later)
	const float *get() const {return m_;}
	float getDeterminant() const;
	bool isInvertable() const;
	static CMatrix3 &identity(CMatrix3 &m);
	static CMatrix3 &translation(const CVec2 t, CMatrix3 &m);
	static CMatrix3 &rotation(const float r, CMatrix3 &m);
	static CMatrix3 &scale(const CVec2 s, CMatrix3 &m);
	void translate(const CVec2 &t);
	void rotate(float r);
	void scale(const CVec2 &t);
	 
	CMatrix3 operator!() const; //Gets inverse
	CMatrix3 operator-(const CMatrix3 &other) const;
	CMatrix3 operator+(const CMatrix3 &other) const;
	CMatrix3 operator*(const CMatrix3 &other) const; //Multiply Matrices
	CMatrix3 &operator*=(const CMatrix3 &other);
	float& operator() (unsigned row, unsigned column); //Set Elements
	float  operator() (unsigned row, unsigned column) const; //Get Elements
private:
	float m_[9];
};
}
//src/runtime/core/public/math/matrix4.h
namespace Arcade
{
class CVec3;
class CMatrix3;
class METACADE_API CMatrix4 
{
public:
	CMatrix4();
	CMatrix4(float m[16]) {set(m);}
	void set(const CVec3 &forward, const CVec3 &right, const CVec3 &up);
	void set(const CMatrix3 &mat3);
	void set(const CMatrix4 &mat4);
	void set(float m[16]); //Temporary (use memcpy later)
	const float *get() const {return m_;}
	void getAxis(CVec3 &forward, CVec3 &right, CVec3 &up) const;
	float getDeterminant() const;
	bool isInvertable() const;
	void getTranspose(CMatrix4 &m);
	void tranformVector(const CVec3 &in, CVec3 &out, bool w) const;
	static CMatrix4 &identity(CMatrix4 &m);
	static CMatrix4 &translation(const CVec3 &t, CMatrix4 &m);
	static CMatrix4 &rotation(const CVec3 &dir, float angle, CMatrix4 &m);
	static CMatrix4 &scale(const CVec3 &s, CMatrix4 &m);
	static CMatrix4 &perspective(float _near, float _far, float fov, float aspect, CMatrix4 &m);
	static CMatrix4 &orthographic(float _near, float _far, float left, float right, float top, float bottom, CMatrix4 &m);
	static CMatrix4 identity();
	static CMatrix4 translation(const CVec3 &t);
	CMatrix4 &scale(const CVec3 &s);
	CMatrix4 &translate(const CVec3 &t);
	CMatrix4 &rotate(const CVec3 &dir, float angle);
	 
	bool operator==(const CMatrix4 &other) const;
	bool operator!=(const CMatrix4 &other) const;
	CMatrix4 operator!() const; //Gets inverse
	CMatrix4 operator-(const CMatrix4 &other) const;
	CMatrix4 operator+(const CMatrix4 &other) const;
	CMatrix4 operator*(const CMatrix4 &other) const; //Multiply Matrices
	CMatrix4 &operator*=(const CMatrix4 &other);
	float& operator() (unsigned row, unsigned column); //Set Elements
	float  operator() (unsigned row, unsigned column) const; //Get Elements
private:
	static void matrix4Mult(const CMatrix4 &a, const CMatrix4 &b, CMatrix4 &c);
	float m_[16];
};
}
//src/runtime/core/public/math/vec2.h
namespace Arcade
{
class CMatrix3;
class METACADE_API CVec2 
{
public:
	union
	{
		struct
		{
			float x,y;
		};
		float _xy[2];
	};
	//Constructor
	CVec2();
	CVec2(float x, float y);
	CVec2(const CVec2 &other);
	//Accessors and Mutators
	void set(float x, float y);
	void set(float xy[2]);
	void set(const CVec2 &other);
	float getX() const;
	float getY() const;
	void get(float *ptr) const;
	//Utility Functions
	float lengthSquared() const;
	float length() const;
	float distanceSquared(const CVec2 &other) const;
	float distance(const CVec2 &other) const;
	float dot(const CVec2 &other) const;
	float cross(const CVec2 &other) const;
	float getAngle() const;
	float angleTo(const CVec2 &other) const;
	bool inRect(const CVec2 &pos, const CVec2 &size) const;
	bool inBox(const CVec2 &min, const CVec2 &max) const;
	CVec2 vmin(const CVec2 &b) const;
	CVec2 vmax(const CVec2 &b) const;
	CVec2 interpolateTo(const CVec2 &other, float fraction) const;
	CVec2 &normalize();
	//Operator Overloads
	 
	float operator[](int i) const;
	float& operator[](int i);
	CVec2 operator--();
	CVec2& operator+=(const CVec2 &other);
	CVec2& operator-=(const CVec2 &other);
	CVec2& operator*=(const CMatrix3 &other);
	CVec2& operator*=(const CVec2 &other);
	CVec2& operator*=(float scalar);
	CVec2& operator/=(const CVec2 &other);
	CVec2& operator/=(float scalar);
	CVec2 operator+(const CVec2 &other) const;
	CVec2 operator-(const CVec2 &other) const;
	CVec2 operator*(const CMatrix3 &other) const;
	CVec2 operator*(const CVec2 &other) const;
	CVec2 operator*(float scalar) const;
	CVec2 operator/(const CVec2 &other) const;
	CVec2 operator/(float scalar) const;
	bool operator==(const CVec2 &other) const;
	bool operator!=(const CVec2 &other) const;
};
CVec2 operator/(float scalar, CVec2 vec);
static const CVec2 Vec2Zero(0,0);
}
//src/runtime/core/public/math/vec3.h
namespace Arcade
{
class CMatrix4;
class METACADE_API CVec3 
{
public:
	union
	{
		struct
		{
			float x,y,z;
		};
		float _xyz[3];
	};
	//Constructor
	CVec3();
	CVec3(float s);
	CVec3(float x, float y, float z);
	CVec3(const CVec3 &other);
	//Accessors and Mutators
	inline void set(float vx, float vy, float vz);
	inline void set(float xyz[3]);
	inline void set(const CVec3 &other);
	inline float getX() const;
	inline float getY() const;
	inline float getZ() const;
	inline void get(float *ptr) const;
	//Utility Functions
	bool equal(const CVec3 &other) const;
	float lengthSquared() const;
	float length() const;
	float distanceSquared(const CVec3 &other) const;
	float distance(const CVec3 &other) const;
	float dot(const CVec3 &other) const;
	CVec3 perpendicular() const;
	CVec3 project(CVec3 normal) const;
	CVec3 cross(const CVec3 &other) const;
	CVec3 getNormal() const;
	CVec3& normalize();
	CVec3 interpolateTo(const CVec3 &other, float fraction) const;
	//Operator Overloads
	 
	float operator[](int i) const;
	float& operator[](int i);
	CVec3 operator--();
	CVec3& operator+=(const CVec3 &other);
	CVec3& operator-=(const CVec3 &other);
	CVec3& operator*=(const CVec3 &other);
	CVec3& operator*=(const CMatrix4 &other);
	CVec3& operator*=(float scalar);
	CVec3& operator/=(const CVec3 &other);
	CVec3& operator/=(float scalar);
	CVec3 operator+(const CVec3 &other) const;
	CVec3 operator-(const CVec3 &other) const;
	CVec3 operator*(const CVec3 &other) const;
	CVec3 operator*(const CMatrix4 &other) const;
	CVec3 operator*(float scalar) const;
	CVec3 operator/(const CVec3 &other) const;
	CVec3 operator/(float scalar) const;
};
static const CVec3 Vec3Zero(0,0,0);
}
//src/runtime/core/public/math/halfplane.h
namespace Arcade
{
class METACADE_API CHalfPlane : public CVec3
{
public:
	CHalfPlane();
	CHalfPlane(const CVec2& dir, float distance);
	CHalfPlane(const CVec2& dir, const CVec2& origin);
	float distance(const CVec2& point) const;
	EPointClassify intersection(const CVec2& start, const CVec2& end, float& fraction) const;
	EPointClassify clasifyPoint(const CVec2& point, bool checkOn = false) const;
};
}
//src/runtime/core/public/gfx/color.h
namespace Arcade
{
struct CFloatColor;
struct METACADE_API CColor
{
	union
	{
		struct
		{
			uint8 r,g,b,a;
		};
		uint8 rgba[4];
	};
	CColor();
	CColor(uint32 irgba);
	CColor(uint8 color[4]);
	CColor(uint8 cr, uint8 cg, uint8 cb, uint8 ca = 0xFF);
	CColor(float fr, float fg, float fb, float fa = 1.0f);
	uint32 asInt() const;
	 
};
struct METACADE_API CFloatColor
{
	union
	{
		struct
		{
			float r,g,b,a;
		};
		float rgba[4];
	};
	CFloatColor();
	CFloatColor(const CColor &color);
	CFloatColor(float fr, float fg, float fb, float fa = 1.0f);
	CFloatColor operator+(const CFloatColor& other) const;
	CFloatColor operator+(float brt) const;
	CFloatColor operator-(const CFloatColor& other) const;
	CFloatColor operator-(float brt) const;
	CFloatColor operator*(float frac) const;
	CFloatColor& operator+=(const CFloatColor& other);
	CFloatColor& operator+=(float brt);
	CFloatColor& operator-=(const CFloatColor& other);
	CFloatColor& operator-=(float brt);
	CFloatColor interpolateTo(const CFloatColor& other, float fraction) const;
	operator CColor() const;
};
}
//src/runtime/core/public/gfx/image.h

//src/runtime/core/public/gfx/vertex.h
namespace Arcade
{
struct CVertex2D;
struct METACADE_API CVertex3D
{
	CVertex3D();
	CVertex3D(const CVec3 &pos, const CVec3 &norm, const CVec2 &tc, const CColor &col = CColor(0xFFFFFFFF));
	CVertex3D(float x, float y, float z,
			float nx, float ny, float nz,
			float s, float t,
			const CColor &col = CColor(0xFFFFFFFF) );
	CVertex3D interpolateTo(const CVertex3D &other, float fraction) const;
	CVec3 _position;
	CVec3 _normal;
	CVec2 _texcoord;
	CColor _color;
};
struct METACADE_API CVertex2D
{
	CVertex2D();
	CVertex2D(const CVec2 &pos, const CVec2 &tc, const CColor &col = CColor(0x000000FF));
	CVertex2D(float x, float y,
			float s, float t,
			const CColor &col = CColor(0xFFFFFFFF));
	CVertex2D(const CVertex3D &other);
	CVertex2D interpolateTo(const CVertex2D &other, float fraction) const;
	CVertex3D to3D() const;
	CVec2 _position;
	CVec2 _texcoord;
	CColor _color;
};
}
//src/runtime/core/public/util/variant.h
namespace Arcade
{
enum EVariantType
{
	VT_NONE,
	VT_BOOLEAN,
	VT_UINT,
	VT_INT,
	VT_DOUBLE,
	VT_STRING,
	VT_MAX,
};
class CVariant
{
public:
	template<typename T>
	CVariant(const T& value)
		: _type(VT_NONE) 
		, _strdata(nullptr)
		, _strrefs(nullptr)
	{
		set(value);
	}
	CVariant();
	CVariant(const CVariant& other);
	~CVariant();
	void set(bool value);
	void set(uint64 value);
	void set(int64 value);
	void set(uint32 value) { set((uint64) value); }
	void set(int32 value) { set((int64) value); }
	void set(uint16 value) { set((uint64) value); }
	void set(int16 value) { set((int64) value); }
	void set(uint8 value) { set((uint64) value); }
	void set(int8 value) { set((int64) value); }
	void set(double value);
	void set(float value);
	void set(const char *value);
	EVariantType type() const;
	const char *getTypeName() const;
	bool get(bool& value) const;
	bool get(uint64& value) const;
	bool get(int64& value) const;
	bool get(uint32& value) const { uint64 v; bool s = get(v); if(s) value = (uint32) v; return s; }
	bool get(int32& value) const { int64 v; bool s = get(v); if(s) value = (int32) v; return s; }
	bool get(uint16& value) const { uint64 v; bool s = get(v); if(s) value = (uint16) v; return s; }
	bool get(int16& value) const { int64 v; bool s = get(v); if(s) value = (int16) v; return s; }
	bool get(uint8& value) const { uint64 v; bool s = get(v); if(s) value = (uint8) v; return s; }
	bool get(int8& value) const { int64 v; bool s = get(v); if(s) value = (int8) v; return s; }
	bool get(double& value) const;
	bool get(const char*& buffer, int32& length) const;
	CVariant& operator = (const CVariant& other);
	template<typename T>
	CVariant& operator = (const T& value)
	{
		set(value);
	}
private:
	void reset();
	EVariantType _type;
	uint8 _pod[8];
	uint8 *_strdata;
	uint32 *_strrefs;
};
}
//src/runtime/core/public/util/guid.h
namespace Arcade
{
class METACADE_API CGUID
{
public:
	CGUID();
	bool operator == (const CGUID& other) const;
	bool operator != (const CGUID& other) const;
	bool operator < (const CGUID& other) const;
	static CGUID generate();
	void reset();
	const char* tostring() const;
private:
	union
	{
		struct
		{
			uint32 A,B,C,D;
		};
		struct
		{
			uint64 X, Y;
		};
	};
};
}
//src/runtime/core/public/util/string.h
namespace Arcade
{
class METACADE_API CString
{
public:
	CString();
	CString(const CString& other);
	CString(const char* str);
	~CString();
	uint32 length() const;
	bool empty() const;
	CString chopLeft(uint32 len) const;
	CString chopRight(uint32 len) const;
	CString sub(uint32 offset, uint32 len) const;
	CString operator+(const CString& other) const;
	CString operator+(const char* other) const;
	CString &operator=(const CString& other);
	bool operator==(const CString& other) const;
	bool operator!=(const CString& other) const;
	bool operator<(const CString& other) const;
	const char* operator*() const;
	const char* get() const;
private:
	CString(uint32 length);
	void reset();
	char* _string;
	uint32 _length;
	uint32* _refs;
};
}
//src/runtime/render/render_public.h
#define MAX_TEXTURE_BITS 10
#define MAX_TEXTURES 1024
#define MAX_VERTICES 65535
#define MAX_INDICES 200000
//src/runtime/render/public/itexture.h
namespace Arcade
{
class ITextureProvider
{
public:
	virtual class ITexture* loadTexture(class IRenderer* renderContext, class IImage* imagesource) = 0;
	virtual void freeTexture(class IRenderer* renderContext, ITexture* texture) = 0;
};
class ITexture
{
public:
	virtual int32 getWidth() const = 0;
	virtual int32 getHeight() const = 0;
	virtual uint16 getID() const = 0;
};
}
//src/runtime/render/public/irenderbuffer.h
namespace Arcade
{
class IRenderBuffer
{
public:
	virtual const CVertex2D* getVertices() const = 0;
	virtual const uint16 getNumVertices() const = 0;
	virtual const uint16* getIndices() const = 0;
	virtual const uint32 getNumIndices() const = 0;
};
}
//src/runtime/render/public/idrawbuffer.h
namespace Arcade
{
class IDrawBuffer
{
public:
	virtual const class CRenderBatch* getRenderBatches() const = 0;
	virtual uint32 getNumRenderBatches() const = 0;
	virtual const class IRenderBuffer* getRenderBuffer() const = 0;
};
}
//src/runtime/render/public/irenderer.h
namespace Arcade
{
class IRenderer
{
public:
	virtual void render(class IDrawBuffer* buffer) = 0;
	virtual class ITextureProvider* getTextureProvider() = 0;
};
}
//src/runtime/render/public/iimage.h
namespace Arcade
{
class IImage
{
public:
	virtual int32 getWidth() const = 0;
	virtual int32 getHeight() const = 0;
	virtual int32 getBytesPerPixel() const = 0;
	virtual EImagePixelFormat getPixelFormat() const = 0;
	virtual uint8* getPixels() const = 0;
	virtual uint32 getUniqueID() const = 0;
};
}
//src/runtime/render/public/material.h
namespace Arcade
{
class METACADE_API CMaterial
{
public:
	CMaterial()
		: _blend(BLEND_NORMAL)
		, _baseTexture(0)
	{}
	EBlendMode _blend;
	uint32 _baseTexture;
	uint64 getHash() const;
};
}
//src/runtime/render/public/renderstate.h
namespace Arcade
{
class METACADE_API CRenderState
{
public:
	CMaterial _material;
	uint64 getHash() const;
	uint32 getStateChangeFlags(const CRenderState& other) const;
};
}
//src/runtime/render/public/renderelement.h
namespace Arcade
{
struct CRenderQuad
{
	CVertex2D _verts[4];
	CRenderQuad& makeBox(const CVec2 &mins, const CVec2 &maxs, const CColor &color)
	{
		_verts[0]._position.set(mins);
		_verts[1]._position.set(maxs.x, mins.y);
		_verts[2]._position.set(maxs);
		_verts[3]._position.set(mins.x, maxs.y);
		_verts[0]._texcoord.set(0,0);
		_verts[1]._texcoord.set(1,0);
		_verts[2]._texcoord.set(1,1);
		_verts[3]._texcoord.set(0,1);
		_verts[0]._color = color;
		_verts[1]._color = color;
		_verts[2]._color = color;
		_verts[3]._color = color;
		return (*this);
	}
	void transform(const CMatrix3 &matrix)
	{
		_verts[0]._position *= matrix;
		_verts[1]._position *= matrix;
		_verts[2]._position *= matrix;
		_verts[3]._position *= matrix;
	}
	void transformUV(const CMatrix3 &matrix)
	{
		_verts[0]._texcoord *= matrix;
		_verts[1]._texcoord *= matrix;
		_verts[2]._texcoord *= matrix;
		_verts[3]._texcoord *= matrix;
	}
};
#define MAX_CLIPPING_PLANES 16
struct CClipShape
{
public:
	CClipShape() 
		: _numPlanes(0)
	{}
	bool add(const CHalfPlane& plane)
	{
		if ( _numPlanes >= MAX_CLIPPING_PLANES - 1 ) return false;
		_planes[_numPlanes++] = plane;
		return true;
	}
	const CHalfPlane& getHalfPlane(int32 i) const { return _planes[i]; }
	int32 getNumPlanes() const { return _numPlanes; }
private:
	CHalfPlane _planes[MAX_CLIPPING_PLANES];
	int32 _numPlanes;
};
class CRenderElement
{
public:
	CRenderElement();
	CRenderElement& makeQuad(const CRenderQuad& quad, const CClipShape& clip, const CRenderState& renderState, int32 layer = 0);
	ERenderElementType getType() const;
	const CRenderQuad& getQuad() const;
	const CRenderState& getRenderState() const;
	const CClipShape& getClipShape() const;
	int32 getLayer() const;
private:
	ERenderElementType _type;
	CRenderState _renderState;
	int32 _layer;
	CRenderQuad _quad;
	CClipShape _clip;
};
}
//src/runtime/render/public/renderbatch.h
namespace Arcade
{
class METACADE_API CRenderBatch
{
public:
	CRenderBatch();
	CRenderBatch(uint32 first, uint32 num, CRenderState renderState, EPrimitive primitive);
	uint32 getFirstIndex() const;
	uint32 getNumIndices() const;
	const class CRenderState getRenderState() const;
	const EPrimitive getPrimitive() const;
	const uint32 getStateChangeFlags() const;
	void createStateChangeFlags(const CRenderState& previousState);
private:
	uint32 _firstIndex;
	uint32 _numIndices;
	CRenderState _renderState;
	EPrimitive _primitive;
	uint32 _stateChangeFlags;
};
}
//src/runtime/engine/engine_public.h
//src/runtime/engine/public/iruntime.h
namespace Arcade
{
class IRenderer;
class IRenderTest
{
public:
	virtual void frame(IRenderer *renderer, float time, CVec2 viewportsize) = 0;
	virtual void start(IRenderer *renderer) = 0;
	virtual void end(IRenderer *renderer) = 0;
	virtual void reloadVM() = 0;
};
class IRuntime
{
public:
	virtual bool initialize(class IRuntimeEnvironment* env) = 0;
	virtual class IPackageManager* getPackageManager() = 0;
	virtual IRenderTest* getRenderTest() = 0;
};
}
//src/runtime/engine/public/ilogger.h
namespace Arcade
{
class ILogger
{
public:
	virtual void log(const char* text, EMessageType type) = 0;
};
}
//src/runtime/engine/public/iallocator.h
namespace Arcade
{
class IAllocator
{
public:
	virtual void* memrealloc(void* pointer, uint32 size) = 0;
	virtual void memfree(void* pointer) = 0;
};
}
//src/runtime/engine/public/ifilesystem.h
namespace Arcade
{
class IFileCollection
{
public:
	virtual void add(const char* filename) = 0;
};
class IFileObject
{
public:
	virtual bool read(void* data, uint32 bytes) = 0;
	virtual bool write(const void* data, uint32 bytes) = 0;
	virtual bool seek(uint32 offset) = 0;
	virtual uint32 tell() = 0;
	virtual uint32 getSize() = 0;
};
class IFileSystem
{
public:
	virtual IFileObject* openFile(const char* filename, EFileIOMode mode) = 0;
	virtual void closeFile(IFileObject* file) = 0;
	//virtual const char** listFilesInDirectory(const char* dir, const char* extFilter = nullptr) = 0;
	virtual bool listFilesInDirectory(IFileCollection* collection, const char* dir, const char* extFilter = nullptr) = 0;
};
}
//src/runtime/engine/public/imachineenvironment.h
namespace Arcade
{
class IMachineEnvironment
{
public:
	virtual class IRenderer* getRenderer() = 0;
	virtual class IAudioSystem* getAudioSystem() = 0;
};
}
//src/runtime/engine/public/iruntimeenvironment.h
namespace Arcade
{
class IRuntimeEnvironment
{
public:
	virtual class IAllocator* getAllocator() = 0;
	virtual class IFileSystem* getFileSystem() = 0;
	virtual class ILogger* getLogger() = 0;
};
}
//src/runtime/engine/public/ipackagemanager.h
namespace Arcade
{
enum EPackageFlags
{
	PACKAGE_LOADED = 0x1,
	PACKAGE_READONLY = 0x2,
};
class IPackage
{
public:
	virtual const char* getPackageName() const = 0;
	virtual uint32 getNumAssets() const = 0;
	virtual class IAsset* getAsset(uint32 index) const = 0;
	virtual void loadAssets() = 0;
	virtual void releaseAssets() = 0;
};
class IPackageManager
{
public:
	virtual class CPackageBuilder* createPackageBuilder() = 0;
	virtual void deletePackageBuilder(class CPackageBuilder* builder) = 0;
	virtual void setRootDirectory(const char* path) = 0;
	virtual const char* getRootDirectory() const = 0;
	virtual bool findAndPreloadPackages() = 0;
	virtual uint32 getNumPackages() const = 0;
	virtual IPackage* getPackage(uint32 index) const = 0;
	virtual void unloadAllPackages() = 0;
};
}
//src/runtime/engine/public/runtimeobject.h
namespace Arcade
{
class METACADE_API CRuntimeObject
{
public:
	CRuntimeObject(class IRuntime* runtime);
	CRuntimeObject(CRuntimeObject* outer);
protected:
	void log(EMessageType type, const char* message, ...);
	void* alloc(unsigned int size);
	void* realloc(void* pointer, unsigned int size);
	void free(void* pointer);
	void free(const void* pointer);
	class IFileObject* openFile(const char* name, EFileIOMode mode);
	void closeFIle(class IFileObject* file);
	bool listFilesInDirectory(class IFileCollection* collection, const char* dir, const char* extFilter = nullptr);
private:
	class IRuntime* _runtime;
};
}
//src/runtime/engine/public/asset.h
namespace Arcade
{
enum EAssetType
{
	ASSET_NONE,
	ASSET_CODE,
	ASSET_TEXTURE,
	ASSET_SOUND,
};
class IAsset
{
public:
	virtual bool load(IFileObject* file) = 0;
	virtual bool save(IFileObject* file) = 0;
	virtual bool validate() const = 0;
	virtual void release() = 0;
	virtual EAssetType getType() const = 0;
	virtual CGUID getUniqueID() const = 0;
	virtual bool isLoaded() const = 0;
	virtual bool isNamedAsset() const = 0;
	virtual CString getName() const = 0;
protected:
	friend class CPackageBuilder;
	friend class CAssetMap;
	virtual void setUniqueID(const CGUID &id) = 0;
	virtual void setLoaded(bool loaded) = 0;
	virtual void setName(const CString& name) = 0;
};
template<EAssetType Type>
class CAsset : public IAsset, public CRuntimeObject
{
public:
	bool checkType() const { return _type == Type; }
	virtual EAssetType getType() const { return _type; }
	virtual CGUID getUniqueID() const { return _uniqueID; }
	virtual bool isLoaded() const { return _loaded; }
	virtual bool isNamedAsset() const { return !_name.empty(); }
	virtual CString getName() const { return _name; }
protected:
	friend class CPackageBuilder;
	friend class CAssetMap;
	CAsset(CRuntimeObject* object) 
		: CRuntimeObject(object)
		, _type(Type)
	{}
	void setUniqueID(const CGUID &id)
	{
		_uniqueID = id;
	}
	void setLoaded(bool loaded)
	{
		_loaded = loaded;
	}
	void setName(const CString& name)
	{
		_name = name;
	}
private:
	EAssetType _type;
	CGUID _uniqueID;
	CString _name;
	bool _loaded;
};
template<typename T>
T* castAsset(IAsset* asset) { if (!asset || !((T*)(asset))->checkType()) return nullptr; return (T*)asset; }
}
//src/runtime/engine/public/packagebuilder.h
namespace Arcade
{
class METACADE_API CPackageBuilder : public CRuntimeObject
{
public:
	~CPackageBuilder();
	template<typename T>
	T* addAsset()
	{
		T* newAsset = new T(this);
		addAsset(newAsset);
		newAsset->setUniqueID(CGUID::generate());
		return newAsset;
	}
	template<typename T>
	T* addNamedAsset(const CString& name)
	{
		T* newAsset = new T(this);
		addAsset(newAsset);
		newAsset->setName(name);
		newAsset->setUniqueID(CGUID::generate());
		return newAsset;
	}
	void removeAsset(class IAsset* asset);
	bool save(const char* packageName);
private:
	friend class CPackageManager;
	void addAsset(class IAsset* asset);
	CPackageBuilder(class CPackage* package);
	class CPackage* _package;
};
}
//src/runtime/engine/public/api.h
namespace Arcade
{
METACADE_API bool create(class IRuntime** runtime);
METACADE_API void destroy(class IRuntime* runtime);
}
//src/runtime/engine/public/assets/scriptresource.h
namespace Arcade
{
class METACADE_API CCodeAsset : public CAsset<ASSET_CODE>
{
public:
	CCodeAsset(CRuntimeObject* outer);
	virtual bool load(IFileObject* file) override;
	virtual bool save(IFileObject* file) override;
	virtual bool validate() const override;
	virtual void release() override;
	const char* getCodeBuffer() const;
	uint32 getCodeLength() const;
	void setCodeBuffer(const char* buffer, uint32 size);
	void setCodeBuffer(const char* buffer);
	char* _code;
	uint32 _codeLength;
};
}
