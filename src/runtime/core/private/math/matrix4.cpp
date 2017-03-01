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
	m_[0] = forward.x;
	m_[1] = forward.y;
	m_[2] = forward.z;
	m_[3] = 0.0f;

	m_[4] = right.x;
	m_[5] = right.y;
	m_[6] = right.z;
	m_[7] = 0.0f;

	m_[8] = up.x;
	m_[9] = up.y;
	m_[10] = up.z;
	m_[11] = 0.0f;

	m_[12] = 0.0f;
	m_[13] = 0.0f;
	m_[14] = 0.0f;
	m_[15] = 1.0f;
}

void CMatrix4::set(const CMatrix3 &mat3)
{
	m_[0] = mat3(0,0);
	m_[1] = mat3(0,1);
	m_[2] = mat3(0,2);

	m_[4] = mat3(1,0);
	m_[5] = mat3(1,1);
	m_[6] = mat3(1,2);

	m_[8] = mat3(2,0);
	m_[9] = mat3(2,1);
	m_[10] = mat3(2,2);

	m_[12] = mat3(2,0);
	m_[13] = mat3(2,1);
	m_[14] = mat3(2,2);
}

void CMatrix4::set(const CMatrix4 &mat4)
{
	memcpy(m_, mat4.m_, 16 * sizeof(float));
}

void CMatrix4::set(float m[16])
{
	for (int i = 0; i < 16; i++) m_[i] = m[i];
}

void CMatrix4::getAxis(CVec3 &forward, CVec3 &right, CVec3 &up) const
{
	forward.x = m_[0];
	forward.y = m_[1];
	forward.z = m_[2];

	right.x = m_[4];
	right.y = m_[5];
	right.z = m_[6];

	up.x = m_[8];
	up.y = m_[9];
	up.z = m_[10];
}

float CMatrix4::getDeterminant() const
{
	float a0 = m_[ 0]*m_[ 5] - m_[ 1]*m_[ 4];
	float a1 = m_[ 0]*m_[ 6] - m_[ 2]*m_[ 4];
	float a2 = m_[ 0]*m_[ 7] - m_[ 3]*m_[ 4];
	float a3 = m_[ 1]*m_[ 6] - m_[ 2]*m_[ 5];
	float a4 = m_[ 1]*m_[ 7] - m_[ 3]*m_[ 5];
	float a5 = m_[ 2]*m_[ 7] - m_[ 3]*m_[ 6];
	float b0 = m_[ 8]*m_[13] - m_[ 9]*m_[12];
	float b1 = m_[ 8]*m_[14] - m_[10]*m_[12];
	float b2 = m_[ 8]*m_[15] - m_[11]*m_[12];
	float b3 = m_[ 9]*m_[14] - m_[10]*m_[13];
	float b4 = m_[ 9]*m_[15] - m_[11]*m_[13];
	float b5 = m_[10]*m_[15] - m_[11]*m_[14];

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
	if(row >= 4 || column >= 4) return m_[0];
	return m_[4 * row + column];
}

float CMatrix4::operator() (unsigned row, unsigned column) const
{
	if(row >= 4 || column >= 4) return 0.0f;
	return m_[4 * row + column];
}

CMatrix4 CMatrix4::operator!() const
{
	float a0 = m_[ 0]*m_[ 5] - m_[ 1]*m_[ 4];
	float a1 = m_[ 0]*m_[ 6] - m_[ 2]*m_[ 4];
	float a2 = m_[ 0]*m_[ 7] - m_[ 3]*m_[ 4];
	float a3 = m_[ 1]*m_[ 6] - m_[ 2]*m_[ 5];
	float a4 = m_[ 1]*m_[ 7] - m_[ 3]*m_[ 5];
	float a5 = m_[ 2]*m_[ 7] - m_[ 3]*m_[ 6];
	float b0 = m_[ 8]*m_[13] - m_[ 9]*m_[12];
	float b1 = m_[ 8]*m_[14] - m_[10]*m_[12];
	float b2 = m_[ 8]*m_[15] - m_[11]*m_[12];
	float b3 = m_[ 9]*m_[14] - m_[10]*m_[13];
	float b4 = m_[ 9]*m_[15] - m_[11]*m_[13];
	float b5 = m_[10]*m_[15] - m_[11]*m_[14];

	float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
	if(fabs(det) > EPSILON)
	{
	    static CMatrix4 inverse;
	    inverse.m_[ 0] = + m_[ 5]*b5 - m_[ 6]*b4 + m_[ 7]*b3;
	    inverse.m_[ 4] = - m_[ 4]*b5 + m_[ 6]*b2 - m_[ 7]*b1;
	    inverse.m_[ 8] = + m_[ 4]*b4 - m_[ 5]*b2 + m_[ 7]*b0;
	    inverse.m_[12] = - m_[ 4]*b3 + m_[ 5]*b1 - m_[ 6]*b0;
	    inverse.m_[ 1] = - m_[ 1]*b5 + m_[ 2]*b4 - m_[ 3]*b3;
	    inverse.m_[ 5] = + m_[ 0]*b5 - m_[ 2]*b2 + m_[ 3]*b1;
	    inverse.m_[ 9] = - m_[ 0]*b4 + m_[ 1]*b2 - m_[ 3]*b0;
	    inverse.m_[13] = + m_[ 0]*b3 - m_[ 1]*b1 + m_[ 2]*b0;
	    inverse.m_[ 2] = + m_[13]*a5 - m_[14]*a4 + m_[15]*a3;
	    inverse.m_[ 6] = - m_[12]*a5 + m_[14]*a2 - m_[15]*a1;
	    inverse.m_[10] = + m_[12]*a4 - m_[13]*a2 + m_[15]*a0;
	    inverse.m_[14] = - m_[12]*a3 + m_[13]*a1 - m_[14]*a0;
	    inverse.m_[ 3] = - m_[ 9]*a5 + m_[10]*a4 - m_[11]*a3;
	    inverse.m_[ 7] = + m_[ 8]*a5 - m_[10]*a2 + m_[11]*a1;
	    inverse.m_[11] = - m_[ 8]*a4 + m_[ 9]*a2 - m_[11]*a0;
	    inverse.m_[15] = + m_[ 8]*a3 - m_[ 9]*a1 + m_[10]*a0;

	    float invDet = 1.0f/det;
	    inverse.m_[ 0] *= invDet;
	    inverse.m_[ 1] *= invDet;
	    inverse.m_[ 2] *= invDet;
	    inverse.m_[ 3] *= invDet;
	    inverse.m_[ 4] *= invDet;
	    inverse.m_[ 5] *= invDet;
	    inverse.m_[ 6] *= invDet;
	    inverse.m_[ 7] *= invDet;
	    inverse.m_[ 8] *= invDet;
	    inverse.m_[ 9] *= invDet;
	    inverse.m_[10] *= invDet;
	    inverse.m_[11] *= invDet;
	    inverse.m_[12] *= invDet;
	    inverse.m_[13] *= invDet;
	    inverse.m_[14] *= invDet;
	    inverse.m_[15] *= invDet;
		return inverse;
	}
	return *this;
}

void CMatrix4::matrix4Mult(const CMatrix4 &a, const CMatrix4 &b, CMatrix4 &c)
{
	c.m_[ 0  ] = a.m_[ 0  ] * b.m_[ 0 ] + a.m_[ 1  ] * b.m_[ 4 ] + a.m_[ 2  ] * b.m_[ 8  ] + a.m_[ 3  ] * b.m_[ 12 ];
	c.m_[ 1  ] = a.m_[ 0  ] * b.m_[ 1 ] + a.m_[ 1  ] * b.m_[ 5 ] + a.m_[ 2  ] * b.m_[ 9  ] + a.m_[ 3  ] * b.m_[ 13 ];
	c.m_[ 2  ] = a.m_[ 0  ] * b.m_[ 2 ] + a.m_[ 1  ] * b.m_[ 6 ] + a.m_[ 2  ] * b.m_[ 10 ] + a.m_[ 3  ] * b.m_[ 14 ];
	c.m_[ 3  ] = a.m_[ 0  ] * b.m_[ 3 ] + a.m_[ 1  ] * b.m_[ 7 ] + a.m_[ 2  ] * b.m_[ 11 ] + a.m_[ 3  ] * b.m_[ 15 ];

	c.m_[ 4  ] = a.m_[ 4  ] * b.m_[ 0 ] + a.m_[ 5  ] * b.m_[ 4 ] + a.m_[ 6  ] * b.m_[ 8  ] + a.m_[ 7  ] * b.m_[ 12 ];
	c.m_[ 5  ] = a.m_[ 4  ] * b.m_[ 1 ] + a.m_[ 5  ] * b.m_[ 5 ] + a.m_[ 6  ] * b.m_[ 9  ] + a.m_[ 7  ] * b.m_[ 13 ];
	c.m_[ 6  ] = a.m_[ 4  ] * b.m_[ 2 ] + a.m_[ 5  ] * b.m_[ 6 ] + a.m_[ 6  ] * b.m_[ 10 ] + a.m_[ 7  ] * b.m_[ 14 ];
	c.m_[ 7  ] = a.m_[ 4  ] * b.m_[ 3 ] + a.m_[ 5  ] * b.m_[ 7 ] + a.m_[ 6  ] * b.m_[ 11 ] + a.m_[ 7  ] * b.m_[ 15 ];

	c.m_[ 8  ] = a.m_[ 8  ] * b.m_[ 0 ] + a.m_[ 9  ] * b.m_[ 4 ] + a.m_[ 10 ] * b.m_[ 8  ] + a.m_[ 11 ] * b.m_[ 12 ];
	c.m_[ 9  ] = a.m_[ 8  ] * b.m_[ 1 ] + a.m_[ 9  ] * b.m_[ 5 ] + a.m_[ 10 ] * b.m_[ 9  ] + a.m_[ 11 ] * b.m_[ 13 ];
	c.m_[ 10 ] = a.m_[ 8  ] * b.m_[ 2 ] + a.m_[ 9  ] * b.m_[ 6 ] + a.m_[ 10 ] * b.m_[ 10 ] + a.m_[ 11 ] * b.m_[ 14 ];
	c.m_[ 11 ] = a.m_[ 8  ] * b.m_[ 3 ] + a.m_[ 9  ] * b.m_[ 7 ] + a.m_[ 10 ] * b.m_[ 11 ] + a.m_[ 11 ] * b.m_[ 15 ];

	c.m_[ 12 ] = a.m_[ 12 ] * b.m_[ 0 ] + a.m_[ 13 ] * b.m_[ 4 ] + a.m_[ 14 ] * b.m_[ 8  ] + a.m_[ 15 ] * b.m_[ 12 ];
	c.m_[ 13 ] = a.m_[ 12 ] * b.m_[ 1 ] + a.m_[ 13 ] * b.m_[ 5 ] + a.m_[ 14 ] * b.m_[ 9  ] + a.m_[ 15 ] * b.m_[ 13 ];
	c.m_[ 14 ] = a.m_[ 12 ] * b.m_[ 2 ] + a.m_[ 13 ] * b.m_[ 6 ] + a.m_[ 14 ] * b.m_[ 10 ] + a.m_[ 15 ] * b.m_[ 14 ];
	c.m_[ 15 ] = a.m_[ 12 ] * b.m_[ 3 ] + a.m_[ 13 ] * b.m_[ 7 ] + a.m_[ 14 ] * b.m_[ 11 ] + a.m_[ 15 ] * b.m_[ 15 ];
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
	for(int i=0; i<16; i++) m.m_[i] = m_[i] - other.m_[i];
	return m;
}

CMatrix4 CMatrix4::operator+(const CMatrix4 &other) const
{
	CMatrix4 m;
	for(int i=0; i<16; i++) m.m_[i] = m_[i] + other.m_[i];
	return m;
}

CMatrix4 &CMatrix4::identity(CMatrix4 &m)
{
	float *m_ = m.m_;
	m_[0] =  1.0f; m_[1] =  0.0f; m_[2] =  0.0f; m_[3] =  0.0f;
	m_[4] =  0.0f; m_[5] =  1.0f; m_[6] =  0.0f; m_[7] =  0.0f;
	m_[8] =  0.0f; m_[9] =  0.0f; m_[10] = 1.0f; m_[11] = 0.0f;
	m_[12] = 0.0f; m_[13] = 0.0f; m_[14] = 0.0f; m_[15] = 1.0f;
	return m;
}

CMatrix4 CMatrix4::identity()
{
	return CMatrix4::identity(CMatrix4());
}

CMatrix4 CMatrix4::translation(const CVec3 &t)
{
	return CMatrix4::translation(t, CMatrix4());
}

CMatrix4 &CMatrix4::translation(const CVec3 &t, CMatrix4 &m)
{
	identity(m);
	m(3,0) = t.x;
	m(3,1) = t.y;
	m(3,2) = t.z;
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
	m(0,0) = s.x;
	m(1,1) = s.y;
	m(2,2) = s.z;
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
	out.x = in.x * m_[0] + in.y * m_[4] + in.z * m_[8];
	out.y = in.x * m_[1] + in.y * m_[5] + in.z * m_[9];
	out.z = in.x * m_[2] + in.y * m_[6] + in.z * m_[10];
	if ( w )
	{
		out.x += m_[12];
		out.y += m_[13];
		out.z += m_[14];
	}
}

CMatrix4 &CMatrix4::translate(const CVec3 &t)
{
	static CMatrix4 a,b;

	a.m_[12] = t.x;
	a.m_[13] = t.y;
	a.m_[14] = t.z;

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

	a.m_[0] = s.x;
	a.m_[5] = s.y;
	a.m_[10] = s.z;

	//Matrix4::scale(s, a);
	matrix4Mult(a, (*this), b); set(b);

	return *this;
}

bool CMatrix4::operator==(const CMatrix4 &other) const
{
	return memcmp(m_,other.m_,16 * sizeof(float)) != 0;
}

bool CMatrix4::operator!=(const CMatrix4 &other) const
{
	return memcmp(m_,other.m_,16 * sizeof(float)) == 0;
}
