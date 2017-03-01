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
matrix4.h: 4x4 matrix
===============================================================================
*/

#pragma once

#include "metacade_types.h"

class CVec3;
class CMatrix3;
class ARCADE_API CMatrix4 
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

	/*friend ostream& operator << (ostream &os, const CMatrix4 &m)
	{
		os << "[";
		os << m.m_[0] << ", " << m.m_[1] << ", " << m.m_[2] << ", " << m.m_[3] << ", " << std::endl;
		os << " " << m.m_[4] << ", " << m.m_[5] << ", " << m.m_[6] << ", " << m.m_[7] << ", " << std::endl;
		os << " " << m.m_[8] << ", " << m.m_[9] << ", " << m.m_[10] << ", " << m.m_[11] << ", " << std::endl;
		os << " " << m.m_[12] << ", " << m.m_[13] << ", " << m.m_[14] << ", " << m.m_[15] << "" << "]";
		return os;
	}*/

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