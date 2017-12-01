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
matrix3.h: 3x3 matrix
===============================================================================
*/

#pragma once

namespace Arcade
{

class CVec2;
class METACADE_API CMatrix3 
{
public:
	CMatrix3();
	CMatrix3(float m[9]) {set(m);}

	void set(float m[9]) {for(int i=0; i<9; i++) _m[i] = m[i];} //Temporary (use memcpy later)
	const float *get() const {return _m;}
	float getDeterminant() const;
	bool isInvertable() const;

	static CMatrix3 &identity(CMatrix3 &m);
	static CMatrix3 &translation(const CVec2 t, CMatrix3 &m);
	static CMatrix3 &rotation(const float r, CMatrix3 &m);
	static CMatrix3 &scale(const CVec2 s, CMatrix3 &m);

	void translate(const CVec2 &t);
	void rotate(float r);
	void scale(const CVec2 &t);

	/*friend std::ostream& operator << (std::ostream &os, const CMatrix3 &m)
	{
		os << "[";
		os << m.m_[0] << ", " << m.m_[1] << ", " << m.m_[2] << ", " << std::endl;
		os << " " << m.m_[3] << ", " << m.m_[4] << ", " << m.m_[5] << ", " << std::endl;
		os << " " << m.m_[6] << ", " << m.m_[7] << ", " << m.m_[8] << ", " << "]";
		return os;
	}*/

	CMatrix3 operator!() const; //Gets inverse
	CMatrix3 operator-(const CMatrix3 &other) const;
	CMatrix3 operator+(const CMatrix3 &other) const;
	CMatrix3 operator*(const CMatrix3 &other) const; //Multiply Matrices
	CMatrix3 &operator*=(const CMatrix3 &other);
	float& operator() (unsigned row, unsigned column); //Set Elements
	float  operator() (unsigned row, unsigned column) const; //Get Elements
private:
	float _m[9];
};

}