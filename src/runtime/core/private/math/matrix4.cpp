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
matrix4.cpp: 4x4 matrix
===============================================================================
*/

#include "core_private.h"

CMatrix4::CMatrix4()
{
	//Creates an identity matrix
	identity(*this);
}

void CMatrix4::set(const CVec3 &forward, const CVec3 &right, const CVec3 &up)
{
	_m[0] = forward._x;
	_m[1] = forward._y;
	_m[2] = forward._z;
	_m[3] = 0.0f;

	_m[4] = right._x;
	_m[5] = right._y;
	_m[6] = right._z;
	_m[7] = 0.0f;

	_m[8] = up._x;
	_m[9] = up._y;
	_m[10] = up._z;
	_m[11] = 0.0f;

	_m[12] = 0.0f;
	_m[13] = 0.0f;
	_m[14] = 0.0f;
	_m[15] = 1.0f;
}

void CMatrix4::set(const CMatrix3 &mat3)
{
	_m[0] = mat3(0,0);
	_m[1] = mat3(0,1);
	_m[2] = mat3(0,2);

	_m[4] = mat3(1,0);
	_m[5] = mat3(1,1);
	_m[6] = mat3(1,2);

	_m[8] = mat3(2,0);
	_m[9] = mat3(2,1);
	_m[10] = mat3(2,2);

	_m[12] = mat3(2,0);
	_m[13] = mat3(2,1);
	_m[14] = mat3(2,2);
}

void CMatrix4::set(const CMatrix4 &mat4)
{
	memcpy(_m, mat4._m, 16 * sizeof(float));
}

void CMatrix4::set(float m[16])
{
	for (int i = 0; i < 16; i++) _m[i] = m[i];
}

void CMatrix4::getAxis(CVec3 &forward, CVec3 &right, CVec3 &up) const
{
	forward._x = _m[0];
	forward._y = _m[1];
	forward._z = _m[2];

	right._x = _m[4];
	right._y = _m[5];
	right._z = _m[6];

	up._x = _m[8];
	up._y = _m[9];
	up._z = _m[10];
}

float CMatrix4::getDeterminant() const
{
	float a0 = _m[ 0]*_m[ 5] - _m[ 1]*_m[ 4];
	float a1 = _m[ 0]*_m[ 6] - _m[ 2]*_m[ 4];
	float a2 = _m[ 0]*_m[ 7] - _m[ 3]*_m[ 4];
	float a3 = _m[ 1]*_m[ 6] - _m[ 2]*_m[ 5];
	float a4 = _m[ 1]*_m[ 7] - _m[ 3]*_m[ 5];
	float a5 = _m[ 2]*_m[ 7] - _m[ 3]*_m[ 6];
	float b0 = _m[ 8]*_m[13] - _m[ 9]*_m[12];
	float b1 = _m[ 8]*_m[14] - _m[10]*_m[12];
	float b2 = _m[ 8]*_m[15] - _m[11]*_m[12];
	float b3 = _m[ 9]*_m[14] - _m[10]*_m[13];
	float b4 = _m[ 9]*_m[15] - _m[11]*_m[13];
	float b5 = _m[10]*_m[15] - _m[11]*_m[14];

	float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
	return det;
}

bool CMatrix4::isInvertable() const
{
	return fabs(getDeterminant()) > EPSILON;
}

void CMatrix4::getTranspose(CMatrix4 &m)
{
	m(0,0) = (*this)(0,0);
	m(1,0) = (*this)(0,1);
	m(2,0) = (*this)(0,2);
	m(3,0) = (*this)(0,3);

	m(0,1) = (*this)(1,0);
	m(1,1) = (*this)(1,1);
	m(2,1) = (*this)(1,2);
	m(3,1) = (*this)(1,3);

	m(0,2) = (*this)(2,0);
	m(1,2) = (*this)(2,1);
	m(2,2) = (*this)(2,2);
	m(3,2) = (*this)(2,3);

	m(0,3) = (*this)(3,0);
	m(1,3) = (*this)(3,1);
	m(2,3) = (*this)(3,2);
	m(3,3) = (*this)(3,3);
}

float& CMatrix4::operator() (unsigned row, unsigned column)
{
	if(row >= 4 || column >= 4) return _m[0];
	return _m[4 * row + column];
}

float CMatrix4::operator() (unsigned row, unsigned column) const
{
	if(row >= 4 || column >= 4) return 0.0f;
	return _m[4 * row + column];
}

CMatrix4 CMatrix4::operator!() const
{
	float a0 = _m[ 0]*_m[ 5] - _m[ 1]*_m[ 4];
	float a1 = _m[ 0]*_m[ 6] - _m[ 2]*_m[ 4];
	float a2 = _m[ 0]*_m[ 7] - _m[ 3]*_m[ 4];
	float a3 = _m[ 1]*_m[ 6] - _m[ 2]*_m[ 5];
	float a4 = _m[ 1]*_m[ 7] - _m[ 3]*_m[ 5];
	float a5 = _m[ 2]*_m[ 7] - _m[ 3]*_m[ 6];
	float b0 = _m[ 8]*_m[13] - _m[ 9]*_m[12];
	float b1 = _m[ 8]*_m[14] - _m[10]*_m[12];
	float b2 = _m[ 8]*_m[15] - _m[11]*_m[12];
	float b3 = _m[ 9]*_m[14] - _m[10]*_m[13];
	float b4 = _m[ 9]*_m[15] - _m[11]*_m[13];
	float b5 = _m[10]*_m[15] - _m[11]*_m[14];

	float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
	if(fabs(det) > EPSILON)
	{
	    static CMatrix4 inverse;
	    inverse._m[ 0] = + _m[ 5]*b5 - _m[ 6]*b4 + _m[ 7]*b3;
	    inverse._m[ 4] = - _m[ 4]*b5 + _m[ 6]*b2 - _m[ 7]*b1;
	    inverse._m[ 8] = + _m[ 4]*b4 - _m[ 5]*b2 + _m[ 7]*b0;
	    inverse._m[12] = - _m[ 4]*b3 + _m[ 5]*b1 - _m[ 6]*b0;
	    inverse._m[ 1] = - _m[ 1]*b5 + _m[ 2]*b4 - _m[ 3]*b3;
	    inverse._m[ 5] = + _m[ 0]*b5 - _m[ 2]*b2 + _m[ 3]*b1;
	    inverse._m[ 9] = - _m[ 0]*b4 + _m[ 1]*b2 - _m[ 3]*b0;
	    inverse._m[13] = + _m[ 0]*b3 - _m[ 1]*b1 + _m[ 2]*b0;
	    inverse._m[ 2] = + _m[13]*a5 - _m[14]*a4 + _m[15]*a3;
	    inverse._m[ 6] = - _m[12]*a5 + _m[14]*a2 - _m[15]*a1;
	    inverse._m[10] = + _m[12]*a4 - _m[13]*a2 + _m[15]*a0;
	    inverse._m[14] = - _m[12]*a3 + _m[13]*a1 - _m[14]*a0;
	    inverse._m[ 3] = - _m[ 9]*a5 + _m[10]*a4 - _m[11]*a3;
	    inverse._m[ 7] = + _m[ 8]*a5 - _m[10]*a2 + _m[11]*a1;
	    inverse._m[11] = - _m[ 8]*a4 + _m[ 9]*a2 - _m[11]*a0;
	    inverse._m[15] = + _m[ 8]*a3 - _m[ 9]*a1 + _m[10]*a0;

	    float invDet = 1.0f/det;
	    inverse._m[ 0] *= invDet;
	    inverse._m[ 1] *= invDet;
	    inverse._m[ 2] *= invDet;
	    inverse._m[ 3] *= invDet;
	    inverse._m[ 4] *= invDet;
	    inverse._m[ 5] *= invDet;
	    inverse._m[ 6] *= invDet;
	    inverse._m[ 7] *= invDet;
	    inverse._m[ 8] *= invDet;
	    inverse._m[ 9] *= invDet;
	    inverse._m[10] *= invDet;
	    inverse._m[11] *= invDet;
	    inverse._m[12] *= invDet;
	    inverse._m[13] *= invDet;
	    inverse._m[14] *= invDet;
	    inverse._m[15] *= invDet;
		return inverse;
	}
	return *this;
}

void CMatrix4::matrix4Mult(const CMatrix4 &a, const CMatrix4 &b, CMatrix4 &c)
{
	c._m[ 0  ] = a._m[ 0  ] * b._m[ 0 ] + a._m[ 1  ] * b._m[ 4 ] + a._m[ 2  ] * b._m[ 8  ] + a._m[ 3  ] * b._m[ 12 ];
	c._m[ 1  ] = a._m[ 0  ] * b._m[ 1 ] + a._m[ 1  ] * b._m[ 5 ] + a._m[ 2  ] * b._m[ 9  ] + a._m[ 3  ] * b._m[ 13 ];
	c._m[ 2  ] = a._m[ 0  ] * b._m[ 2 ] + a._m[ 1  ] * b._m[ 6 ] + a._m[ 2  ] * b._m[ 10 ] + a._m[ 3  ] * b._m[ 14 ];
	c._m[ 3  ] = a._m[ 0  ] * b._m[ 3 ] + a._m[ 1  ] * b._m[ 7 ] + a._m[ 2  ] * b._m[ 11 ] + a._m[ 3  ] * b._m[ 15 ];

	c._m[ 4  ] = a._m[ 4  ] * b._m[ 0 ] + a._m[ 5  ] * b._m[ 4 ] + a._m[ 6  ] * b._m[ 8  ] + a._m[ 7  ] * b._m[ 12 ];
	c._m[ 5  ] = a._m[ 4  ] * b._m[ 1 ] + a._m[ 5  ] * b._m[ 5 ] + a._m[ 6  ] * b._m[ 9  ] + a._m[ 7  ] * b._m[ 13 ];
	c._m[ 6  ] = a._m[ 4  ] * b._m[ 2 ] + a._m[ 5  ] * b._m[ 6 ] + a._m[ 6  ] * b._m[ 10 ] + a._m[ 7  ] * b._m[ 14 ];
	c._m[ 7  ] = a._m[ 4  ] * b._m[ 3 ] + a._m[ 5  ] * b._m[ 7 ] + a._m[ 6  ] * b._m[ 11 ] + a._m[ 7  ] * b._m[ 15 ];

	c._m[ 8  ] = a._m[ 8  ] * b._m[ 0 ] + a._m[ 9  ] * b._m[ 4 ] + a._m[ 10 ] * b._m[ 8  ] + a._m[ 11 ] * b._m[ 12 ];
	c._m[ 9  ] = a._m[ 8  ] * b._m[ 1 ] + a._m[ 9  ] * b._m[ 5 ] + a._m[ 10 ] * b._m[ 9  ] + a._m[ 11 ] * b._m[ 13 ];
	c._m[ 10 ] = a._m[ 8  ] * b._m[ 2 ] + a._m[ 9  ] * b._m[ 6 ] + a._m[ 10 ] * b._m[ 10 ] + a._m[ 11 ] * b._m[ 14 ];
	c._m[ 11 ] = a._m[ 8  ] * b._m[ 3 ] + a._m[ 9  ] * b._m[ 7 ] + a._m[ 10 ] * b._m[ 11 ] + a._m[ 11 ] * b._m[ 15 ];

	c._m[ 12 ] = a._m[ 12 ] * b._m[ 0 ] + a._m[ 13 ] * b._m[ 4 ] + a._m[ 14 ] * b._m[ 8  ] + a._m[ 15 ] * b._m[ 12 ];
	c._m[ 13 ] = a._m[ 12 ] * b._m[ 1 ] + a._m[ 13 ] * b._m[ 5 ] + a._m[ 14 ] * b._m[ 9  ] + a._m[ 15 ] * b._m[ 13 ];
	c._m[ 14 ] = a._m[ 12 ] * b._m[ 2 ] + a._m[ 13 ] * b._m[ 6 ] + a._m[ 14 ] * b._m[ 10 ] + a._m[ 15 ] * b._m[ 14 ];
	c._m[ 15 ] = a._m[ 12 ] * b._m[ 3 ] + a._m[ 13 ] * b._m[ 7 ] + a._m[ 14 ] * b._m[ 11 ] + a._m[ 15 ] * b._m[ 15 ];
}

CMatrix4 CMatrix4::operator*(const CMatrix4 &other) const
{
	static CMatrix4 o;

	matrix4Mult( *this, other, o );

	return o;
}

CMatrix4 &CMatrix4::operator*=(const CMatrix4 &other)
{
	static CMatrix4 o;

	matrix4Mult( *this, other, o );
	set(o);

	return *this;
}

CMatrix4 CMatrix4::operator-(const CMatrix4 &other) const
{
	CMatrix4 m;
	for(int i=0; i<16; i++) m._m[i] = _m[i] - other._m[i];
	return m;
}

CMatrix4 CMatrix4::operator+(const CMatrix4 &other) const
{
	CMatrix4 m;
	for(int i=0; i<16; i++) m._m[i] = _m[i] + other._m[i];
	return m;
}

CMatrix4 &CMatrix4::identity(CMatrix4 &m)
{
	float *m_ = m._m;
	m_[0] =  1.0f; m_[1] =  0.0f; m_[2] =  0.0f; m_[3] =  0.0f;
	m_[4] =  0.0f; m_[5] =  1.0f; m_[6] =  0.0f; m_[7] =  0.0f;
	m_[8] =  0.0f; m_[9] =  0.0f; m_[10] = 1.0f; m_[11] = 0.0f;
	m_[12] = 0.0f; m_[13] = 0.0f; m_[14] = 0.0f; m_[15] = 1.0f;
	return m;
}

CMatrix4 CMatrix4::identity()
{
	CMatrix4 out;
	return CMatrix4::identity(out);
}

CMatrix4 CMatrix4::translation(const CVec3 &t)
{
	CMatrix4 out;
	return CMatrix4::translation(t, out);
}

CMatrix4 &CMatrix4::translation(const CVec3 &t, CMatrix4 &m)
{
	identity(m);
	m(3,0) = t._x;
	m(3,1) = t._y;
	m(3,2) = t._z;
	return m;
}
	
CMatrix4 &CMatrix4::rotation(const CVec3 &dir, float angle, CMatrix4 &m)
{
	float c = cosf(angle);
	float s = sinf(angle);

	CVec3 axis = dir.getNormal();
	CVec3 temp = axis * (1.0f - c);

	identity(m);

	m(0, 0) = c + temp[0] * axis[0];
	m(0, 1) = 0 + temp[0] * axis[1] + s * axis[2];
	m(0, 2) = 0 + temp[0] * axis[2] - s * axis[1];

	m(1, 0) = 0 + temp[1] * axis[0] - s * axis[2];
	m(1, 1) = c + temp[1] * axis[1];
	m(1, 2) = 0 + temp[1] * axis[2] + s * axis[0];

	m(2, 0) = 0 + temp[2] * axis[0] + s * axis[1];
	m(2, 1) = 0 + temp[2] * axis[1] - s * axis[0];
	m(2, 2) = c + temp[2] * axis[2];

	return m;
}

CMatrix4 &CMatrix4::scale(const CVec3 &s, CMatrix4 &m)
{
	identity(m);
	m(0,0) = s._x;
	m(1,1) = s._y;
	m(2,2) = s._z;
	return m;
}

inline float rad(float in) { return in * (M_FPI / 180.0f); }

CMatrix4 &CMatrix4::perspective(float _near, float _far, float fov, float aspect, CMatrix4 &m)
{
	float scale = 1.0f / tan(rad(fov * 0.5f));
	m(0,0) = scale / aspect;
	m(1,1) = scale;
	m(2,2) = (_near + _far) / (_near - _far);
	m(2,3) = -1.0f;
	m(3,2) = (2.0f * _near * _far)  / (_near - _far);
	m(3,3) = 0.0f;
	return m;
}

CMatrix4 &CMatrix4::orthographic(float _near, float _far, float left, float right, float top, float bottom, CMatrix4 &m)
{
	m(0,0) = 2.0f / (right - left);
	m(1,1) = 2.0f / (top - bottom);
	m(2,2) = 2.0f / (_near - _far);
	m(3,3) = 1.0f;

	m(3,0) = (left + right)/(left - right);
	m(3,1) = (bottom + top)/(bottom - top);
	m(3,2) = (_near + _far) / (_far - _near);
	return m;
}

void CMatrix4::tranformVector(const CVec3 &in, CVec3 &out, bool w) const
{
	out._x = in._x * _m[0] + in._y * _m[4] + in._z * _m[8];
	out._y = in._x * _m[1] + in._y * _m[5] + in._z * _m[9];
	out._z = in._x * _m[2] + in._y * _m[6] + in._z * _m[10];
	if ( w )
	{
		out._x += _m[12];
		out._y += _m[13];
		out._z += _m[14];
	}
}

CMatrix4 &CMatrix4::translate(const CVec3 &t)
{
	static CMatrix4 a,b;

	a._m[12] = t._x;
	a._m[13] = t._y;
	a._m[14] = t._z;

	matrix4Mult(a, (*this), b); set(b);

	return *this;
}

CMatrix4 &CMatrix4::rotate(const CVec3 &dir, float angle)
{
	static CMatrix4 a, b;

	CMatrix4::rotation(dir, angle, a);
	matrix4Mult(a, (*this), b); set(b);

	return *this;
}

CMatrix4 &CMatrix4::scale(const CVec3 &s)
{
	static CMatrix4 a,b;

	a._m[0] = s._x;
	a._m[5] = s._y;
	a._m[10] = s._z;

	//Matrix4::scale(s, a);
	matrix4Mult(a, (*this), b); set(b);

	return *this;
}

bool CMatrix4::operator==(const CMatrix4 &other) const
{
	return memcmp(_m,other._m,16 * sizeof(float)) != 0;
}

bool CMatrix4::operator!=(const CMatrix4 &other) const
{
	return memcmp(_m,other._m,16 * sizeof(float)) == 0;
}
