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
	_elements.reserve(8192);
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
	return _elements.back();
}

void CRenderList::pop()
{
	_elements.pop_back();
}

bool CRenderList::empty()
{
	return _elements.empty();
}

void CRenderList::sort()
{
	std::sort(_elements.begin(), _elements.end(), [](const CRenderElement &a, const CRenderElement &b)
	{
		int32 layerA = a.getLayer();
		int32 layerB = b.getLayer();

		if ( layerA != layerB ) return layerA > layerB;

		uint64 hashA = a.getRenderState().getHash();
		uint64 hashB = b.getRenderState().getHash();

		return hashA > hashB;
	});
}
