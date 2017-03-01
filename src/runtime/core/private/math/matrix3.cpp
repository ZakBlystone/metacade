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
matrix3.cpp: 3x3 matrix
===============================================================================
*/

#include "core_private.h"

CMatrix3::CMatrix3()
{
	//Creates an identity matrix
	identity(*this);
}

float CMatrix3::getDeterminant() const
{
	/*
		[ 0] [ 1] [ 2]
		[ 3] [ 4] [ 5]
		[ 6] [ 7] [ 8]
	*/
	CMatrix3 get = *this;

	float a0 = m_[0] * (m_[ 4]*m_[ 8] - m_[ 7]*m_[ 5]);
	float a1 = m_[1] * (m_[ 6]*m_[ 5] - m_[ 3]*m_[ 8]);
	float a2 = m_[2] * (m_[ 3]*m_[ 7] - m_[ 6]*m_[ 4]);

	return a0 + a1 + a2;
    /*return (get(0, 0) * (get(1, 1) * get(2, 2) - get(2, 1) * get(1, 2)) +
            get(0, 1) * (get(2, 0) * get(1, 2) - get(1, 0) * get(2, 2)) +
            get(0, 2) * (get(1, 0) * get(2, 1) - get(2, 0) * get(1, 1)));*/
}

bool CMatrix3::isInvertable() const
{
	return fabs(getDeterminant()) > EPSILON;
}

float& CMatrix3::operator() (unsigned row, unsigned column)
{
	if(row >= 3 || column >= 3) return m_[0];
	return m_[3 * row + column];
}

float CMatrix3::operator() (unsigned row, unsigned column) const
{
	if(row >= 3 || column >= 3) return 0.0f;
	return m_[3 * row + column];
}

CMatrix3 CMatrix3::operator!() const
{
	float det = getDeterminant();
	if(fabs(det) > EPSILON)
	{
		static CMatrix3 inverse;
		static CMatrix3 cofactor;

		float inv_det = 1.0f / det;

		/*
			[ 0] [ 1] [ 2]
			[ 3] [ 4] [ 5]
			[ 6] [ 7] [ 8]
		*/

		cofactor(0,0) = m_[ 4]*m_[ 8] - m_[ 7]*m_[ 5];
		cofactor(0,1) = m_[ 6]*m_[ 5] - m_[ 3]*m_[ 8];
		cofactor(0,2) = m_[ 3]*m_[ 7] - m_[ 6]*m_[ 4];
		cofactor(1,0) = m_[ 7]*m_[ 2] - m_[ 1]*m_[ 8];
		cofactor(1,1) = m_[ 0]*m_[ 8] - m_[ 6]*m_[ 2];
		cofactor(1,2) = m_[ 6]*m_[ 1] - m_[ 0]*m_[ 7];
		cofactor(2,0) = m_[ 1]*m_[ 5] - m_[ 4]*m_[ 2];
		cofactor(2,1) = m_[ 3]*m_[ 2] - m_[ 0]*m_[ 5];
		cofactor(2,2) = m_[ 0]*m_[ 4] - m_[ 3]*m_[ 1];

		inverse(0, 0) = cofactor(0, 0) * inv_det;
		inverse(0, 1) = cofactor(1, 0) * inv_det;
		inverse(0, 2) = cofactor(2, 0) * inv_det;

		inverse(1, 0) = cofactor(0, 1) * inv_det;
		inverse(1, 1) = cofactor(1, 1) * inv_det;
		inverse(1, 2) = cofactor(2, 1) * inv_det;

		inverse(2, 0) = cofactor(0, 2) * inv_det;
		inverse(2, 1) = cofactor(1, 2) * inv_det;
		inverse(2, 2) = cofactor(2, 2) * inv_det;

		return inverse;
	}
	return *this;
}

CMatrix3 CMatrix3::operator*(const CMatrix3 &other) const
{
	int		i, j;
	CMatrix3 o;

	for ( i = 0 ; i < 3 ; i++ ) {
		for ( j = 0 ; j < 3 ; j++ ) {
			o.m_[ i * 3 + j ] =
				m_[ i * 3 + 0 ] * other.m_[ 0 * 3 + j ]
				+ m_[ i * 3 + 1 ] * other.m_[ 1 * 3 + j ]
				+ m_[ i * 3 + 2 ] * other.m_[ 2 * 3 + j ];
		}
	}

	return o;
}

CMatrix3 &CMatrix3::operator*=(const CMatrix3 &other)
{
	*this = *this * other;
	return *this;
}

CMatrix3 CMatrix3::operator-(const CMatrix3 &other) const
{
	CMatrix3 m;
	for(int i=0; i<9; i++) m.m_[i] = m_[i] - other.m_[i];
	return m;
}

CMatrix3 CMatrix3::operator+(const CMatrix3 &other) const
{
	CMatrix3 m;
	for(int i=0; i<9; i++) m.m_[i] = m_[i] + other.m_[i];
	return m;
}

CMatrix3 &CMatrix3::identity(CMatrix3 &m)
{
	float *m_ = m.m_;
	m_[0] =  1.0f; m_[1] =  0.0f; m_[2] =  0.0f;
	m_[3] =  0.0f; m_[4] =  1.0f; m_[5] =  0.0f;
	m_[6] =  0.0f; m_[7] =  0.0f; m_[8] = 1.0f;
	return m;
}

CMatrix3 &CMatrix3::translation(const CVec2 t, CMatrix3 &m)
{
	identity(m);
	m(0,2) = t.getX();
	m(1,2) = t.getY();
	return m;
}

CMatrix3 &CMatrix3::rotation(const float r, CMatrix3 &m)
{
	float sin = sinf(r);
	float cos = cosf(r);

	identity(m);

	m(0,0) = cos;
	m(0,1) = sin;
	m(1,0) = -sin;
	m(1,1) = cos;

	return m;
}
	
CMatrix3 &CMatrix3::scale(const CVec2 s, CMatrix3 &m)
{
	identity(m);
	m(0,0) = s.getX();
	m(1,1) = s.getY();
	m(2,2) = 1.0f;
	return m;
}

void CMatrix3::translate(const CVec2 &t)
{
	static CMatrix3 m;
	translation(t, m);
	(*this) = m * (*this);
}

void CMatrix3::rotate(float r)
{
	static CMatrix3 m;
	rotation(r, m);
	(*this) = m * (*this);
}

void CMatrix3::scale(const CVec2 &t)
{
	static CMatrix3 m;
	scale(t, m);
	(*this) = m * (*this);
}