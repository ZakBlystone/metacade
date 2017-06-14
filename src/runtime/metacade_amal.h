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
#define EPSILON 0.00001f
#ifndef M_PI
#define M_PI       3.14159265358979323846f
#endif
#define M_FPI	   3.1415926f
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
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
	inline CVec2();
	inline CVec2(float x, float y);
	inline CVec2(const CVec2 &other);
	//Accessors and Mutators
	inline void set(float x, float y);
	inline void set(float xy[2]);
	inline void set(const CVec2 &other);
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
	inline void vmin(const CVec2 &b);
	inline void vmax(const CVec2 &b);
	inline CVec2 interpolateTo(const CVec2 &other, float fraction) const;
	static inline void interpolateTo(const CVec2& A, const CVec2& B, CVec2& result, float fraction);
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
	inline CVec3();
	inline CVec3(float s);
	inline CVec3(float x, float y, float z);
	inline CVec3(const CVec3 &other);
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
	//Constructs a half-plane, values are uninitialized
	inline CHalfPlane();
	//Constructs a half-plane from a direction and a distance
	inline CHalfPlane(const CVec2& dir, float distance);
	//Constructs a half-plane from a direction and a point on the plane (origin)
	inline CHalfPlane(const CVec2& dir, const CVec2& origin);
	//Calculates the distance between the plane and 'point'
	inline float distance(const CVec2& point) const;
	//Calculates the intersection of the line [start, end] and the plane
	//Only returns intersection if start->end faces into the plane from the front
	//TOI is stored in 'fraction', returns type of intersection
	inline EPointClassify intersection(const CVec2& start, const CVec2& end, float& fraction) const;
	//Returns whether or not 'point' is behind or in front of the plane
	//Passing template parameter 'true' for 'CheckOn' will cause this to also return if the point is on the plane
	template<bool CheckOn = false>
	inline EPointClassify classifyPoint(const CVec2& point) const
	{
		if ( distance(point) <= -EPSILON ) return PLANE_BEHIND;
		return PLANE_INFRONT;
	}
};
//Template specialization for 'classifyPoint' that returns it the point is on the plane as well
template<>
inline EPointClassify CHalfPlane::classifyPoint<true>(const CVec2& point) const
{
	float dist = distance(point);
	if ( dist <= -EPSILON ) return PLANE_BEHIND;
	if ( dist <= EPSILON ) return PLANE_ON;
	return PLANE_INFRONT;
}
}
//src/runtime/core/public/gfx/color.h
namespace Arcade
{
struct CFloatColor;
//24-bit color represented in little endian RGBA format [ 0xRRGGBBAA ]
struct METACADE_API CColor
{
	union
	{
		struct
		{
			uint8 r,g,b,a;
		};
		uint8 rgba[4];
		uint32 irgba;
	};
	//Construct an empty CColor object, initializes as opaque black (0x000000FF)
	inline CColor();
	//Construct from an RGBA packed integer
	inline CColor(uint32 irgba);
	//Construct from an 8-bit RGBA array
	inline CColor(uint8 color[4]);
	//Construct from four 8-bit values (alpha defaults to opaque)
	inline CColor(uint8 cr, uint8 cg, uint8 cb, uint8 ca = 0xFF);
	//Converts CColor to a packed RGBA integer
	uint32 asInt() const;
	 
};
//color represented as floating point values per channel
struct METACADE_API CFloatColor
{
	union
	{
		struct
		{
			float r,g,b,a;
		};
		float rgba[4];
		uint32 iRGBA;
	};
	//Construct an empty CFloatColor object, initializes as opaque black (0x000000FF)
	inline CFloatColor();
	//Construct from a 24-bit CColor
	inline CFloatColor(const CColor &color);
	//Construct from four floating point values (alpha defaults to opaque)
	inline CFloatColor(float fr, float fg, float fb, float fa = 1.0f);
	//Arithmetic operators
	CFloatColor operator+(const CFloatColor& other) const;
	CFloatColor operator+(float brt) const;
	CFloatColor operator-(const CFloatColor& other) const;
	CFloatColor operator-(float brt) const;
	CFloatColor operator*(float frac) const;
	CFloatColor& operator+=(const CFloatColor& other);
	CFloatColor& operator+=(float brt);
	CFloatColor& operator-=(const CFloatColor& other);
	CFloatColor& operator-=(float brt);
	//Interpolates between two colors
	inline CFloatColor interpolateTo(const CFloatColor& other, float fraction) const;
	//Conversion operator to convert to 24-bit CColor
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
	inline CVertex2D();
	inline CVertex2D(const CVec2 &pos, const CVec2 &tc, const CColor &col = CColor(0x000000FF));
	inline CVertex2D(float x, float y,
			float s, float t,
			const CColor &col = CColor(0xFFFFFFFF));
	inline CVertex2D(const CVertex3D &other);
	inline CVertex2D interpolateTo(const CVertex2D &other, float fraction) const;
	static void interpolateTo(const CVertex2D& A, const CVertex2D& B, CVertex2D& result, float fraction);
	CVertex3D to3D() const;
	CVec2 _position;
	CVec2 _texcoord;
	CColor _color;
};
}
//src/runtime/core/public/util/refcounter.h
namespace Arcade
{
class METACADE_API CReferenceCounter
{
public:
	CReferenceCounter()
		: _ref(new uint32(1))
	{}
	CReferenceCounter(const CReferenceCounter& other)
		: _ref(other._ref)
	{
		(*_ref)++;
	}
	~CReferenceCounter()
	{
		reset();
	}
	CReferenceCounter &operator=(const CReferenceCounter& other)
	{
		if ( &other == this ) return *this;
		reset();
		_ref = other._ref;
		(*_ref)++;
		return *this;
	}
	bool unique() const { return _ref == nullptr || (*_ref) <= 1; }
	uint32 count() const { return _ref == nullptr ? 0 : (*_ref); }
private:
	void reset()
	{
		if ( --(*_ref) == 0 )
		{
			delete _ref;
			_ref = nullptr;
		}
	}
	uint32 *_ref;
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
class METACADE_API CVariant
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
	void set(const class CString& str);
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
	bool get(class CString& str) const;
	CVariant& operator = (const CVariant& other);
	template<typename T>
	CVariant& operator = (const T& value)
	{
		set(value);
		return *this;
	}
	CString toString() const;
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
	//Construct empty GUID
	CGUID();
	//Comparison operators
	bool operator == (const CGUID& other) const;
	bool operator != (const CGUID& other) const;
	bool operator < (const CGUID& other) const;
	//Check if GUID is valid (i.e. non-zero)
	bool isValid() const;
	//Generate a new GUID using the appropriate platform function
	static CGUID generate();
	//Set GUID to zero
	void reset();
	//Convert GUID to string using %08X-%08X-%08X-%08X pattern
	const char* tostring() const;
	//Union to hold 64-bit and 32-bit parts of the GUID
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
	bool startsWith(const CString& string) const;
	bool endsWith(const CString& string) const;
	bool contains(const CString& string) const;
	//returns -1 if not found
	int32 find(const CString& string) const;
	CString chopLeft(uint32 len) const;
	CString chopRight(uint32 len) const;
	CString sub(uint32 offset, uint32 len) const;
	CString lower() const;
	CString upper() const;
	CString operator/(const CString& other) const;
	CString operator/(const char* other) const;
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
//src/runtime/core/public/util/sha.h
namespace Arcade
{
class CSHA1
{
public:
	CSHA1();
	typedef union
	{
		uint8 c[64];
		uint32 l[16];
	} WorkspaceBlock;
	class Generator
	{
	public:
		Generator()
			: _block ((WorkspaceBlock* ) _workspace)
		{}
		void init();
		void update(const void* data, uint32 length);
		void done();
		CSHA1 get() const;
	private:
		void transform(uint32* state, const uint8* buffer);
		uint32 _state[5];
		uint32 _count[2];
		uint32 reserved0;
		uint8 _buffer[64];
		uint8 _digest[20];
		uint32 _reserved1[3];
		uint8 _workspace[64];
		WorkspaceBlock* _block;
	};
	void getDigest(uint8 digest[20]) const;
	CString tostring() const;
private:
	uint8 _digest[20];
};
}
//src/runtime/sound/sound_public.h
//src/runtime/sound/public/sample.h
namespace Arcade
{
struct CSampleInfo
{
	uint32 numFrames;
	int32 numChannels;
	int32 sampleRate;
	int32 width;
};
class ISoundSample
{
public:
	virtual void getSampleInfo(CSampleInfo& info) = 0;
	virtual uint8* getPCMSamples() = 0;
};
}
//src/runtime/sound/public/iaudio.h
namespace Arcade
{
class IAudioSystem
{
	virtual ~IAudioSystem() {};
};
}
//src/runtime/sound/public/ichannel.h
namespace Arcade
{
enum EChannelID
{
	CHANNEL_ANY = -1,
	CHANNEL_INVALID = 0xFFFFFFFF
};
enum EChannelMode
{
	CHANNELMODE_DEFAULT = 0,
	CHANNELMODE_PERSISTENT,
};
class ISoundChannel
{
public:
	virtual ~ISoundChannel() {};
};
}
//src/runtime/sound/public/imixer.h
#define MIXF_STEREO 1 << 0
#define MIXF_ALIASED_INTERPOLATION 1 << 1
#define MIXF_8BIT 1 << 2
#define MIXF_DEFAULT MIXF_STEREO
namespace Arcade
{
struct CMixerSettings
{
	CMixerSettings()
		: flags(MIXF_DEFAULT)
		, sampleRate(44100)
		, bufferSize(1024)
		, maxChannels(32)
	{}
	int32 flags;
	int32 sampleRate;
	int32 bufferSize;
	int32 maxChannels;
	inline int32 getBytesPerFrame() const
	{
		return ((flags & MIXF_STEREO) ? 2 : 1) * ((flags & MIXF_8BIT) ? 1 : 2);
	}
	inline int32 getChannelCount() const
	{
		return ((flags & MIXF_STEREO) ? 2 : 1);
	}
};
class ISoundMixer
{
public:
	virtual uint32 playSound(const class CAssetRef &sound, int32 channel = EChannelID::CHANNEL_ANY) = 0;
	virtual void stopSound(int32 channel) = 0;
	virtual uint32 createPersistentChannel() = 0;
	virtual void destroyPersistentChannel(uint32 channel) = 0;
	virtual void update() = 0;
	virtual uint8* getPCMSamples() = 0;
	virtual uint32 availablePCMData() = 0;
	virtual void setChannelPitch(int32 channel, float pitch) = 0;
	virtual void setChannelLooping(int32 channel, bool loop) = 0;
	virtual void setChannelVolume(int32 channel, float volume) = 0;
	virtual ~ISoundMixer() {};
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
	virtual ~ITextureProvider() {};
};
class ITexture
{
public:
	virtual int32 getWidth() const = 0;
	virtual int32 getHeight() const = 0;
	virtual uint16 getID() const = 0;
	virtual ~ITexture() {};
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
	virtual ~IRenderBuffer() {};
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
	virtual ~IDrawBuffer() {};
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
	virtual ~IRenderer() {};
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
	virtual uint32 getID() const = 0;
	virtual ~IImage() = 0;
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
	inline void makeBox(const CVec2 &mins, const CVec2 &maxs, const CColor &color)
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
	inline CClipShape() 
		: _numPlanes(0)
	{}
	inline bool add(const CHalfPlane& plane)
	{
		if ( _numPlanes >= MAX_CLIPPING_PLANES - 1 ) return false;
		_planes[_numPlanes++] = plane;
		return true;
	}
	inline const CHalfPlane& getHalfPlane(int32 i) const { return _planes[i]; }
	inline int32 getNumPlanes() const { return _numPlanes; }
private:
	CHalfPlane _planes[MAX_CLIPPING_PLANES];
	int32 _numPlanes;
};
class CRenderElement
{
public:
	CRenderElement();
	CRenderElement& makeQuad(const CRenderQuad& quad, const CClipShape& clip, const CRenderState& renderState, int32 layer = 0);
	CRenderQuad& makeQuad2(const CClipShape& clip, const CRenderState& state, int32 layer = 0);
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
//src/runtime/engine/public/hidkeys.h
namespace Arcade
{
#define HID_KEYCODE(x) KEY_##x,
enum EHIDKeyCode
{
//src/runtime/engine/public/../hidkeys.inl
HID_KEYCODE(RESERVED0)
	HID_KEYCODE(ERROR_ROLLOVER)
	HID_KEYCODE(POSTFAIL)
	HID_KEYCODE(ERRORUNDEFINED)
	HID_KEYCODE(A)
	HID_KEYCODE(B)
	HID_KEYCODE(C)
	HID_KEYCODE(D)
	HID_KEYCODE(E)
	HID_KEYCODE(F)
	HID_KEYCODE(G)
	HID_KEYCODE(H)
	HID_KEYCODE(I)
	HID_KEYCODE(J)
	HID_KEYCODE(K)
	HID_KEYCODE(L)
	HID_KEYCODE(M)
	HID_KEYCODE(N)
	HID_KEYCODE(O)
	HID_KEYCODE(P)
	HID_KEYCODE(Q)
	HID_KEYCODE(R)
	HID_KEYCODE(S)
	HID_KEYCODE(T)
	HID_KEYCODE(U)
	HID_KEYCODE(V)
	HID_KEYCODE(W)
	HID_KEYCODE(X)
	HID_KEYCODE(Y)
	HID_KEYCODE(Z)
	HID_KEYCODE(1)
	HID_KEYCODE(2)
	HID_KEYCODE(3)
	HID_KEYCODE(4)
	HID_KEYCODE(5)
	HID_KEYCODE(6)
	HID_KEYCODE(7)
	HID_KEYCODE(8)
	HID_KEYCODE(9)
	HID_KEYCODE(0)
	HID_KEYCODE(RETURN)
	HID_KEYCODE(ESCAPE)
	HID_KEYCODE(BACKSPACE)
	HID_KEYCODE(TAB)
	HID_KEYCODE(SPACEBAR)
	HID_KEYCODE(MINUS)
	HID_KEYCODE(EQUALS)
	HID_KEYCODE(LEFT_BRACKET)
	HID_KEYCODE(RIGHT_BRACKET)
	HID_KEYCODE(BACKSLASH)
	HID_KEYCODE(POUND)
	HID_KEYCODE(SEMICOLON)
	HID_KEYCODE(QUOTE)
	HID_KEYCODE(TILDE)
	HID_KEYCODE(COMMA)
	HID_KEYCODE(PERIOD)
	HID_KEYCODE(FORWARDSLASH)
	HID_KEYCODE(CAPSLOCK)
	HID_KEYCODE(F1)
	HID_KEYCODE(F2)
	HID_KEYCODE(F3)
	HID_KEYCODE(F4)
	HID_KEYCODE(F5)
	HID_KEYCODE(F6)
	HID_KEYCODE(F7)
	HID_KEYCODE(F8)
	HID_KEYCODE(F9)
	HID_KEYCODE(F10)
	HID_KEYCODE(F11)
	HID_KEYCODE(F12)
	HID_KEYCODE(PRINTSCR)
	HID_KEYCODE(SCRLOCK)
	HID_KEYCODE(PAUSE)
	HID_KEYCODE(INSERT)
	HID_KEYCODE(HOME)
	HID_KEYCODE(PAGEUP)
	HID_KEYCODE(DELETE)
	HID_KEYCODE(END)
	HID_KEYCODE(PAGEDOWN)
	HID_KEYCODE(RIGHT)
	HID_KEYCODE(LEFT)
	HID_KEYCODE(DOWN)
	HID_KEYCODE(UP)
	HID_KEYCODE(KP_NUMLOCK)
	HID_KEYCODE(KP_FORWARDSLASH)
	HID_KEYCODE(KP_ASTERISK)
	HID_KEYCODE(KP_MINUS)
	HID_KEYCODE(KP_PLUS)
	HID_KEYCODE(KP_ENTER)
	HID_KEYCODE(KP_1)
	HID_KEYCODE(KP_2)
	HID_KEYCODE(KP_3)
	HID_KEYCODE(KP_4)
	HID_KEYCODE(KP_5)
	HID_KEYCODE(KP_6)
	HID_KEYCODE(KP_7)
	HID_KEYCODE(KP_8)
	HID_KEYCODE(KP_9)
	HID_KEYCODE(KP_0)
	HID_KEYCODE(KP_PERIOD)
	HID_KEYCODE(BACKSLASH2)
	HID_KEYCODE(APPLICATION)
	HID_KEYCODE(POWER)
	HID_KEYCODE(KP_EQUALS)
	HID_KEYCODE(F13)
	HID_KEYCODE(F14)
	HID_KEYCODE(F15)
	HID_KEYCODE(F16)
	HID_KEYCODE(F17)
	HID_KEYCODE(F18)
	HID_KEYCODE(F19)
	HID_KEYCODE(F20)
	HID_KEYCODE(F21)
	HID_KEYCODE(F22)
	HID_KEYCODE(F23)
	HID_KEYCODE(F24)
	HID_KEYCODE(EXECUTE)
	HID_KEYCODE(HELP)
	HID_KEYCODE(MENU)
	HID_KEYCODE(SELECT)
	HID_KEYCODE(STOP)
	HID_KEYCODE(AGAIN)
	HID_KEYCODE(UNDO)
	HID_KEYCODE(CUT)
	HID_KEYCODE(COPY)
	HID_KEYCODE(PASTE)
	HID_KEYCODE(FIND)
	HID_KEYCODE(MUTE)
	HID_KEYCODE(VOLUME_UP)
	HID_KEYCODE(VOLUME_DOWN)
	HID_KEYCODE(LOCKING_CAPS)
	HID_KEYCODE(LOCKING_NUM)
	HID_KEYCODE(LOCKING_SCROLL)
	HID_KEYCODE(KP_COMMA)
	HID_KEYCODE(KP_EQUALSIGN)
	HID_KEYCODE(INTERNATIONAL_1)
	HID_KEYCODE(INTERNATIONAL_2)
	HID_KEYCODE(INTERNATIONAL_3)
	HID_KEYCODE(INTERNATIONAL_4)
	HID_KEYCODE(INTERNATIONAL_5)
	HID_KEYCODE(INTERNATIONAL_6)
	HID_KEYCODE(INTERNATIONAL_7)
	HID_KEYCODE(INTERNATIONAL_8)
	HID_KEYCODE(INTERNATIONAL_9)
	HID_KEYCODE(LANG_1)
	HID_KEYCODE(LANG_2)
	HID_KEYCODE(LANG_3)
	HID_KEYCODE(LANG_4)
	HID_KEYCODE(LANG_5)
	HID_KEYCODE(LANG_6)
	HID_KEYCODE(LANG_7)
	HID_KEYCODE(LANG_8)
	HID_KEYCODE(LANG_9)
	HID_KEYCODE(ALTERNATE_ERASE)
	HID_KEYCODE(SYSREQ)
	HID_KEYCODE(CANCEL)
	HID_KEYCODE(CLEAR)
	HID_KEYCODE(PRIOR)
	HID_KEYCODE(RETURN2)
	HID_KEYCODE(SEPARATOR)
	HID_KEYCODE(OUT)
	HID_KEYCODE(OPER)
	HID_KEYCODE(CLEAR2)
	HID_KEYCODE(CRSEL)
	HID_KEYCODE(EXSEL)
	HID_KEYCODE(RESERVED1)
	HID_KEYCODE(RESERVED2)
	HID_KEYCODE(RESERVED3)
	HID_KEYCODE(RESERVED4)
	HID_KEYCODE(RESERVED5)
	HID_KEYCODE(RESERVED6)
	HID_KEYCODE(RESERVED7)
	HID_KEYCODE(RESERVED8)
	HID_KEYCODE(RESERVED9)
	HID_KEYCODE(RESERVED10)
	HID_KEYCODE(RESERVED11)
	HID_KEYCODE(KP_00)
	HID_KEYCODE(KP_000)
	HID_KEYCODE(THOUSANDS_SEPARATOR)
	HID_KEYCODE(DECIMAL_SEPARATOR)
	HID_KEYCODE(CURRENCY_UNIT)
	HID_KEYCODE(CURRENCY_SUBUNIT)
	HID_KEYCODE(KP_LEFTPAREN)
	HID_KEYCODE(KP_RIGHTPAREN)
	HID_KEYCODE(KP_LEFTBRACE)
	HID_KEYCODE(KP_RIGHTBRACE)
	HID_KEYCODE(KP_TAB)
	HID_KEYCODE(KP_BACKSPACE)
	HID_KEYCODE(KP_A)
	HID_KEYCODE(KP_B)
	HID_KEYCODE(KP_C)
	HID_KEYCODE(KP_D)
	HID_KEYCODE(KP_E)
	HID_KEYCODE(KP_F)
	HID_KEYCODE(KP_XOR)
	HID_KEYCODE(KP_CARET)
	HID_KEYCODE(KP_PERCENT)
	HID_KEYCODE(KP_LESS)
	HID_KEYCODE(KP_GREATER)
	HID_KEYCODE(KP_AMPERSAND)
	HID_KEYCODE(KP_DOUBLEAMPERSAND)
	HID_KEYCODE(KP_BAR)
	HID_KEYCODE(KP_DOUBLEBAR)
	HID_KEYCODE(KP_COLON)
	HID_KEYCODE(KP_POUND)
	HID_KEYCODE(KP_SPACE)
	HID_KEYCODE(KP_AT)
	HID_KEYCODE(KP_EXCLAMATIONPOINT)
	HID_KEYCODE(KP_MEMSTORE)
	HID_KEYCODE(KP_MEMRECALL)
	HID_KEYCODE(KP_MEMCLEAR)
	HID_KEYCODE(KP_MEMADD)
	HID_KEYCODE(KP_MEMSUBTRACT)
	HID_KEYCODE(KP_MEMMULTIPLY)
	HID_KEYCODE(KP_MEMDIVIDE)
	HID_KEYCODE(KP_PLUSMINUS)
	HID_KEYCODE(KP_CLEAR)
	HID_KEYCODE(KP_CLEARENTRY)
	HID_KEYCODE(KP_BINARY)
	HID_KEYCODE(KP_OCTAL)
	HID_KEYCODE(KP_DECIMAL)
	HID_KEYCODE(KP_HEXADECIMAL)
	HID_KEYCODE(RESERVED12)
	HID_KEYCODE(RESERVED13)
	HID_KEYCODE(LEFT_CONTROL)
	HID_KEYCODE(LEFT_SHIFT)
	HID_KEYCODE(LEFT_ALT)
	HID_KEYCODE(LEFT_GUI)
	HID_KEYCODE(RIGHT_CONTROL)
	HID_KEYCODE(RIGHT_SHIFT)
	HID_KEYCODE(RIGHT_ALT)
	HID_KEYCODE(RIGHT_GUI)
	KEY_MAX,
};
#undef HID_KEYCODE
class METACADE_API CHIDKeyCodes
{
public:
	static CString getKeyName(EHIDKeyCode code);
};
}
//src/runtime/engine/public/inputstate.h
namespace Arcade
{
enum EMouseButton
{
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_X1,
	MOUSE_BUTTON_X2,
	MOUSE_BUTTON_MAX,
};
enum EFocusElement
{
	FOCUS_KEYBOARD,
	FOCUS_MOUSE,
	FOCUS_MAX,
};
enum EInputStateFlags
{
	INPUTSTATE_NONE,
	INPUTSTATE_MOUSEBUTTONS = 1 << 0,
	INPUTSTATE_MOUSEPOSITION = 1 << 1,
	INPUTSTATE_KEYBOARD = 1 << 2,
	INPUTSTATE_FOCUS = 1 << 3,
	INPUTSTATE_ALL = (1 << 4) - 1,
};
class METACADE_API CInputState
{
public:
	CInputState();
	void clear();
	void setKeyboardIsFocused(bool focused);
	void setMouseIsFocused(bool focused);
	void setMousePosition(float x, float y);
	void setMouseButton(EMouseButton button, bool pressed);
	void setKey(uint8 keycode, bool pressed);
	bool getKeyboardIsFocused() const;
	bool getMouseIsFocused() const;
	void getMousePosition(float &x, float &y) const;
	bool getMouseButtonIsDown(EMouseButton button) const;
	bool getKeyIsDown(uint8 keycode) const;
	uint8 getMouseButtonBits() const
	{
		return _mouseButtons;
	}
	void applyEvent(const class CInputEvent& eventData);
	void merge(const CInputState& other);
	template<typename Predicate>
	void generateEvents(const CInputState& previous, Predicate func) const
	{
		//mouse buttons
		if ( _stateFlags & EInputStateFlags::INPUTSTATE_MOUSEBUTTONS )
		{
			for ( int32 i=0; i<EMouseButton::MOUSE_BUTTON_MAX; ++i )
			{
				bool oldState = (previous._mouseButtons & (1 << i)) != 0;
				bool newState = (_mouseButtons & (1 << i)) != 0;
				if ( oldState != newState )
				{
					func( CInputEvent::generateMouseButtonEvent( (EMouseButton) i, newState ) );
				}
			}
		}
		//mouse position
		if ( _stateFlags & EInputStateFlags::INPUTSTATE_MOUSEPOSITION )
		{
			if ( previous._mouseX != _mouseX || previous._mouseY != _mouseY )
			{
				func( CInputEvent::generateMouseMovementEvent( previous._mouseX, previous._mouseY, _mouseX, _mouseY ) );
			}
		}
		//keys
		if ( _stateFlags & EInputStateFlags::INPUTSTATE_KEYBOARD )
		{
			for ( int32 i=0; i<0xFF; ++i )
			{
				if ( previous._keyboard[i] != _keyboard[i] )
				{
					func( CInputEvent::generateKeyEvent( i, _keyboard[i] == 1 ) );
				}
			}
		}
		//focus
		if ( _stateFlags & EInputStateFlags::INPUTSTATE_FOCUS )
		{
			for ( int32 i=0; i<EFocusElement::FOCUS_MAX; ++i )
			{
				bool oldState = (previous._focus & (1 << i)) != 0;
				bool newState = (_focus & (1 << i)) != 0;
				if ( oldState != newState )
				{
					func( CInputEvent::generateFocusEvent( (EFocusElement) i, newState ) );
				}
			}
		}
	}
	uint8 getStateFlags() const;
	void setStateFlags(uint8 state);
private:
	void setFocusElement(EFocusElement focusElement, bool focused);
	bool getFocusElement(EFocusElement focusElement) const;
	uint8 _keyboard[0xFF]; //state of all keys
	uint8 _stateFlags;
	uint8 _mouseButtons;
	uint8 _focus;
	float _mouseX;
	float _mouseY;
};
}
//src/runtime/engine/public/inputevent.h
namespace Arcade
{
enum EInputEventType
{
	INPUTEVENT_NONE,
	INPUTEVENT_MOUSEPRESSED,
	INPUTEVENT_MOUSERELEASED,
	INPUTEVENT_MOUSEMOVED,
	INPUTEVENT_KEYPRESSED,
	INPUTEVENT_KEYRELEASED,
	INPUTEVENT_FOCUSGAINED,
	INPUTEVENT_FOCUSLOST,
};
class METACADE_API CInputEvent
{
public:
	static CInputEvent generateKeyEvent(uint8 keycode, bool wasPressed);
	static CInputEvent generateMouseButtonEvent(EMouseButton button, bool wasPressed);
	static CInputEvent generateMouseMovementEvent(float oldX, float oldY, float x, float y);
	static CInputEvent generateFocusEvent(EFocusElement element, bool hasFocus);
	EInputEventType getEventType() const;
	EFocusElement getFocusElement() const;
	EMouseButton getMouseButton() const;
	uint8 getKeycode() const;
	float getMouseX() const;
	float getMouseY() const;
	float getMouseDeltaX() const;
	float getMouseDeltaY() const;
private:
	CInputEvent();
	EInputEventType _eventType;
	EFocusElement _focusElement;
	EMouseButton _mouseButton;
	uint8 _keycode;
	float _mouseX;
	float _mouseY;
	float _mouseDeltaX;
	float _mouseDeltaY;
};
}
//src/runtime/engine/public/functioncall.h
namespace Arcade
{
class METACADE_API CFunctionCall
{
public:
	CFunctionCall();
	CFunctionCall(const CString& func);
	~CFunctionCall();
	template<typename...Args>
	CFunctionCall(const CString& func, const Args&... args)
		: CFunctionCall(func)
	{
		int32 length = sizeof...(args);
		CVariant vals[] = {args...};
		for ( int32 i=0; i<length; ++i )
			addArg(vals[i]);
	}
	uint32 numArgs() const;
	CVariant getArg(uint32 i) const;
	CString getFunction() const;
	void setFunction(const CString& func);
	void addArg(const CVariant& v);
private:
	CString _func;
	CReferenceCounter _counter;
	void* _args;
};
}
//src/runtime/engine/public/igame.h
namespace Arcade
{
class IGameClass
{
public:
	virtual bool createInstance(class IGameInstance** instance) = 0;
	virtual void deleteInstance(class IGameInstance* instance) = 0;
	virtual ~IGameClass() {};
};
class IHostCallbacks
{
public:
	virtual bool handleHostFunctionCall(const CFunctionCall& call, CVariant& returnValue) = 0;
	virtual ~IHostCallbacks() {};
};
class IGameInstance
{
public:
	virtual class IGameClass* getClass() = 0;
	virtual void postInputEvent(const class CInputEvent& input) = 0;
	virtual void postInputState(const class CInputState& input) = 0;
	virtual void init() = 0;
	virtual void think(float time) = 0;
	virtual void render(class IRenderer* renderer, CVec2 viewportSize, uint32 targetID = 0) = 0;
	virtual void initializeRenderer(class IRenderer* renderer) = 0;
	virtual void finishRenderer(class IRenderer* renderer) = 0;
	virtual bool callFunction(const CFunctionCall& call) = 0;
	virtual void initializeTextures(class ITextureProvider* provider) = 0;
	virtual void finishTextures(class ITextureProvider* provider) = 0;
	virtual void initSoundMixer(const CMixerSettings& settings) = 0;
	virtual ISoundMixer* getSoundMixer() = 0;
	virtual void setHostCallbacks(IHostCallbacks* callbacks) = 0;
	virtual ~IGameInstance() {};
};
}
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
	virtual void callFunction(CFunctionCall call) = 0;
	virtual ~IRenderTest() {}
};
class IMetaData;
class IRuntime
{
public:
	virtual bool initialize(class IRuntimeEnvironment* env) = 0;
	virtual class IPackageManager* getPackageManager() = 0;
	virtual IRenderTest* createRenderTest() = 0;
	virtual void deleteRenderTest(IRenderTest* test) = 0;
	virtual IMetaData* createMetaData() = 0;
	virtual void deleteMetaData(IMetaData* data) = 0;
	virtual ISoundMixer* createSoundMixer(CMixerSettings settings) = 0;
	virtual void deleteSoundMixer(ISoundMixer* mixer) = 0;
	virtual class IGameClass* getGameClassForPackage(class IPackage* package) = 0;
	virtual ~IRuntime() {}
};
}
//src/runtime/engine/public/ilogger.h
namespace Arcade
{
class ILogger
{
public:
	virtual void log(const char* text, EMessageType type) = 0;
	virtual ~ILogger() {}
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
	virtual ~IAllocator() {}
};
}
//src/runtime/engine/public/ifilesystem.h
namespace Arcade
{
class IFileCollection
{
public:
	virtual void add(const CString& filename) = 0;
	virtual ~IFileCollection() {}
};
class IFileObject
{
public:
	virtual bool read(void* data, uint32 bytes) = 0;
	virtual bool write(const void* data, uint32 bytes) = 0;
	virtual bool seek(uint32 offset) = 0;
	virtual uint32 tell() = 0;
	virtual uint32 getSize() = 0;
	virtual ~IFileObject() {}
};
class IFileSystem
{
public:
	virtual IFileObject* openFile(const CString& filename, EFileIOMode mode) = 0;
	virtual void closeFile(IFileObject* file) = 0;
	//virtual const char** listFilesInDirectory(const char* dir, const char* extFilter = nullptr) = 0;
	virtual bool listFilesInDirectory(IFileCollection* collection, const char* dir, const char* extFilter = nullptr) = 0;
	virtual ~IFileSystem() {}
};
}
//src/runtime/engine/public/imetadata.h
namespace Arcade
{
class IMetaData
{
public:
	virtual uint32 getNumKeys() const = 0;
	virtual CString getKey(uint32 i) const = 0;
	virtual CVariant getValue(uint32 i) const = 0;
	virtual CVariant getValue(const CString& key) const = 0;
	virtual void setKeyValuePair(const CString& key, const CVariant& value) = 0;
	virtual ~IMetaData() {};
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
	virtual ~IMachineEnvironment() {};
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
	virtual ~IRuntimeEnvironment() {};
};
}
//src/runtime/engine/public/ipackagemanager.h
namespace Arcade
{
enum EPackageFlags
{
	PACKAGE_LOADED = 0x1,
	PACKAGE_READONLY = 0x2,
	PACKAGE_PRELOADED = 0x4,
};
class IPackage
{
public:
	virtual CString getPackageName() const = 0;
	virtual CGUID getPackageID() const = 0;
	virtual uint32 getNumAssets() const = 0;
	virtual CAssetRef getAsset(uint32 index) const = 0;
	virtual const IMetaData* getMetaData() const = 0;
	virtual bool loadAssets() = 0;
	virtual void releaseAssets() = 0;
	virtual CAssetRef findAssetByID(const CGUID& id) const = 0;
	virtual CAssetRef findAssetByName(const CString& name) const = 0;
	virtual ~IPackage() {}
};
class IPackageManager
{
public:
	virtual class CPackageBuilder* createPackageBuilder(const CString& packageName = "unnamed") = 0;
	virtual void deletePackageBuilder(class CPackageBuilder* builder) = 0;
	virtual void setRootDirectory(const CString& path) = 0;
	virtual CString getRootDirectory() const = 0;
	virtual bool findAndPreloadPackages() = 0;
	virtual uint32 getNumPackages() const = 0;
	virtual IPackage* getPackage(uint32 index) const = 0;
	virtual IPackage* getPackageByName(const CString& name) const = 0;
	virtual IPackage* getPackageByID(const CGUID& id) const = 0;
	virtual void unloadAllPackages() = 0;
	virtual ~IPackageManager() {}
};
}
//src/runtime/engine/public/runtimeobject.h
namespace Arcade
{
//Ideally, this shouldn't be exported, it's just quicker than making interfaces for everything
class METACADE_API CRuntimeObject
{
public:
	CRuntimeObject(class IRuntime* runtime);
	CRuntimeObject(CRuntimeObject* outer);
protected:
#ifdef ENGINE_PRIVATE
	template<typename T, typename... ArgT> T *construct(ArgT&&... args)
	{
		return new(zalloc(sizeof(T))) T(args...);
	}
	template<typename T>
	void destroy(T* obj)
	{
		if ( obj == nullptr ) return;
		obj->~T();
		zfree(obj);
	}
	//not great, but only compiles when you use it, so whatever I'll fix it later
	template <typename T, typename... ArgT> shared_ptr<T> makeShared(ArgT&&... args)
	{
		return shared_ptr<T>
			( this->construct<T>( args... )
			, [this](T* ptr) { this->destroy(ptr); });
	}
	template<typename T>
	T* castAsset(const CAssetRef& ref) 
	{ 
		IAsset* asset = ref.get(_runtime); 
		if (!asset || !((T*)(asset))->checkType()) return nullptr; 
		return (T*)asset; 
	}
	void log(EMessageType type, const char* message, ...);
	void* zalloc(uint32 size);
	void* zrealloc(void* pointer, uint32 size);
	void zfree(void* pointer);
	void zfree(const void* pointer);
	class IFileObject* openFile(const CString& name, EFileIOMode mode);
	void closeFIle(class IFileObject* file);
	bool listFilesInDirectory(class IFileCollection* collection, const char* dir, const char* extFilter = nullptr);
	class IRuntime* getRuntime() const;
	class CIndex allocateImageIndex();
	class IVMHost* getLuaVM();
#endif
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
	ASSET_ANY,
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
	virtual ~IAsset() = 0;
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
class METACADE_API CAssetRef
{
public:
	CAssetRef();
	EAssetType getType() const;
	IAsset* get(IRuntime* runtime) const;
	IPackage* getPackage(IRuntime* runtime) const;
	CGUID getAssetID() const;
	CGUID getPackageID() const;
private:
	friend class CPackage;
	CAssetRef(class CPackage* package, IAsset* asset);
	CGUID _asset, _package;
	EAssetType _type;
};
}
//src/runtime/engine/public/packagebuilder.h
namespace Arcade
{
class IAssetCompiler
{
public:
	virtual bool compile(IAsset* asset, class IMetaData* buildParameters) = 0;
};
class METACADE_API CPackageBuilder : public CRuntimeObject
{
public:
	~CPackageBuilder();
	template<typename T>
	T* addAsset()
	{
		T* newAsset = new T(this);
		newAsset->setUniqueID(CGUID::generate());
		addAsset(newAsset);
		return newAsset;
	}
	template<typename T>
	T* addNamedAsset(const CString& name)
	{
		T* newAsset = new T(this);
		newAsset->setName(name);
		newAsset->setUniqueID(CGUID::generate());
		addAsset(newAsset);
		return newAsset;
	}
	void setAssetCompiler(IAssetCompiler* compiler);
	IAssetCompiler* getAssetCompiler();
	void removeAsset(class IAsset* asset);
	IMetaData* getMetaData();
	class IAsset* findAssetByName(const CString& name);
	class IAsset* findAssetById(const CGUID& id);
	bool load();
	bool save();
private:
	friend class CPackageManager;
	void addAsset(class IAsset* asset);
	CPackageBuilder(class CPackage* package);
	IAssetCompiler* _compiler;
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
//src/runtime/engine/public/assets/imageresource.h
namespace Arcade
{
class METACADE_API CImageAsset : public CAsset<ASSET_TEXTURE>, public IImage
{
public:
	CImageAsset(CRuntimeObject* outer);
	virtual ~CImageAsset();
	virtual bool load(IFileObject* file) override;
	virtual bool save(IFileObject* file) override;
	virtual bool validate() const override;
	virtual void release() override;
	virtual int32 getWidth() const override;
	virtual int32 getHeight() const override;
	virtual int32 getBytesPerPixel() const override;
	virtual EImagePixelFormat getPixelFormat() const override;
	virtual uint8* getPixels() const override;
	virtual uint32 getID() const override;
	void setImagePixels(EImagePixelFormat format, uint8 bpc, int32 width, int32 height, uint8* pixels);
private:
	class CIndex *_index;
	EImagePixelFormat _format;
	uint8 _bpc;
	int32 _width;
	int32 _height;
	uint8* _pixels;
};
}
//src/runtime/engine/public/assets/soundresource.h
namespace Arcade
{
class METACADE_API CSoundAsset : public CAsset<ASSET_SOUND>, public ISoundSample
{
public:
	CSoundAsset(CRuntimeObject* outer);
	virtual ~CSoundAsset();
	virtual bool load(IFileObject* file) override;
	virtual bool save(IFileObject* file) override;
	virtual bool validate() const override;
	virtual void release() override;
	void setWaveData(uint8* waveData, uint32 waveSize);
	void setSampleInfo(const CSampleInfo& info);
	 
	virtual void getSampleInfo(CSampleInfo& info) override;
	virtual uint8* getPCMSamples() override;
private:
	uint8* _waveData;
	uint32 _waveSize;
	CSampleInfo _sampleInfo;
};
}
