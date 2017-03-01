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
renderlist.cpp:
===============================================================================
*/

#include "render_private.h"

CRenderList::CRenderList()
{
}

void CRenderList::appendRenderElement(const CRenderElement& element)
{
	_elements.push_back(element);
}

CRenderElement& CRenderList::emplaceRenderElement()
{
	_elements.emplace_back();
	return _elements.back();
}

void CRenderList::clear()
{
	_elements.clear();
}

CRenderElement& CRenderList::top()
{
	return _elements.front();
}

void CRenderList::pop()
{
	_elements.pop_front();
}

bool CRenderList::empty()
{
	return _elements.empty();
}

void CRenderList::sort()
{
	_elements.sort([](const CRenderElement &a, const CRenderElement &b)
	{
		if ( a._layer != b._layer ) return a._layer > b._layer;

		return a._renderState.getHash() > b._renderState.getHash();
	});
}
