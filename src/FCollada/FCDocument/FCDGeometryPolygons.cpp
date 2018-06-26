/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the FS Import classes:
	Copyright (C) 2005-2006 Feeling Software Inc
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDExtra.h"
#include "FCDocument/FCDGeometryMesh.h"
#include "FCDocument/FCDGeometryPolygons.h"
#include "FCDocument/FCDGeometryPolygonsInput.h"
#include "FCDocument/FCDGeometrySource.h"

//
// FCDGeometryPolygons
//

ImplementObjectType(FCDGeometryPolygons);
ImplementParameterObject(FCDGeometryPolygons, FCDGeometryPolygonsInput, inputs, new FCDGeometryPolygonsInput(parent->GetDocument(), parent));
ImplementParameterObject(FCDGeometryPolygons, FCDExtra, extra, new FCDExtra(parent->GetDocument(), parent));

FCDGeometryPolygons::FCDGeometryPolygons(FCDocument* document, FCDGeometryMesh* _parent)
:	FCDObject(document)
,	m_Parent(_parent)
,	InitializeParameterNoArg(m_Inputs)
,	InitializeParameterNoArg(m_FaceVertexCounts)
,	InitializeParameterNoArg(m_HoleFaces)
,	InitializeParameter(m_PrimitiveType, POLYGONS)
,	faceVertexCount(0), faceOffset(0), faceVertexOffset(0), holeOffset(0)
,	InitializeParameterNoArg(materialSemantic)
,	InitializeParameterNoArg(extra)
{
	// Pre-buffer the face-vertex counts so that AddFaceVertexCount won't be extremely costly.
	m_FaceVertexCounts.reserve(32);
}

FCDGeometryPolygons::~FCDGeometryPolygons()
{
	m_HoleFaces.clear();
	m_Parent = nullptr;
}

FCDExtra* FCDGeometryPolygons::GetExtra()
{
	return (extra != nullptr) ? extra : (extra = new FCDExtra(GetDocument(), this));
}

// Creates a new face.
void FCDGeometryPolygons::AddFace(uint32 degree)
{
	bool newPolygonSet = m_FaceVertexCounts.empty();
	m_FaceVertexCounts.push_back(degree);

	// Inserts empty indices
	size_t inputCount = m_Inputs.size();
	for (size_t i = 0; i < inputCount; ++i)
	{
		FCDGeometryPolygonsInput* input = m_Inputs[i];
		if (!newPolygonSet && input->OwnsIndices()) input->SetIndexCount(input->GetIndexCount() + degree);
		else if (newPolygonSet && input->GetIndexCount() == 0)
		{
			// Declare this input as the owner!
			input->SetIndexCount(degree);
		}
	}

	m_Parent->Recalculate();
	SetDirtyFlag();
}

// Removes a face
void FCDGeometryPolygons::RemoveFace(size_t index)
{
	FUAssert(index < GetFaceCount(), return);

	// Remove the associated indices, if they exist.
	size_t offset = GetFaceVertexOffset(index);
	size_t indexCount = GetFaceVertexCount(index);
	size_t inputCount = m_Inputs.size();
	for (size_t i = 0; i < inputCount; ++i)
	{
		FCDGeometryPolygonsInput* input = m_Inputs[i];
		if (!input->OwnsIndices()) continue;

		size_t inputIndexCount = input->GetIndexCount();
		if (offset < inputIndexCount)
		{
			// Move the indices backwards.
			uint32* indices = input->GetIndices();
			for (size_t o = offset; o < inputIndexCount - indexCount; ++o)
			{
				indices[o] = indices[o + indexCount];
			}
			input->SetIndexCount(max(offset, inputIndexCount - indexCount));
		}
	}

	// Remove the face and its holes
	size_t holeBefore = GetHoleCountBefore(index);
	size_t holeCount = GetHoleCount(index);
	m_FaceVertexCounts.erase(index + holeBefore, holeCount + 1); // +1 in order to remove the polygon as well as the holes.

	m_Parent->Recalculate();
	SetDirtyFlag();
}

// Calculates the offset of face-vertex pairs before the given face index within the polygon set.
size_t FCDGeometryPolygons::GetFaceVertexOffset(size_t index) const
{
	size_t offset = 0;

	// We'll need to skip over the holes
	size_t holeCount = GetHoleCountBefore(index);
	if (index + holeCount < m_FaceVertexCounts.size())
	{
		// Sum up the wanted offset
		UInt32List::const_iterator end = m_FaceVertexCounts.begin() + index + holeCount;
		for (UInt32List::const_iterator it = m_FaceVertexCounts.begin(); it != end; ++it)
		{
			offset += (*it);
		}
	}
	return offset;
}

// Calculates the number of holes within the polygon set that appear before the given face index.
size_t FCDGeometryPolygons::GetHoleCountBefore(size_t index) const
{
	size_t holeCount = 0;
	for (UInt32List::const_iterator it = m_HoleFaces.begin(); it != m_HoleFaces.end(); ++it)
	{
		if ((*it) <= index) { ++holeCount; ++index; }
	}
	return holeCount;
}

// Retrieves the number of holes within a given face.
size_t FCDGeometryPolygons::GetHoleCount(size_t index) const
{
	size_t holeCount = 0;
	for (size_t i = index + GetHoleCountBefore(index) + 1; i < m_FaceVertexCounts.size(); ++i)
	{
		bool isHoled = m_HoleFaces.find((uint32) i) != m_HoleFaces.end();
		if (!isHoled) break;
		else ++holeCount;
	}
	return holeCount;
}

// The number of face-vertex pairs for a given face.
size_t FCDGeometryPolygons::GetFaceVertexCount(size_t index) const
{
	size_t count = 0;
	if (index < GetFaceCount())
	{
		size_t holeCount = GetHoleCount(index);
		UInt32List::const_iterator it = m_FaceVertexCounts.begin() + index + GetHoleCountBefore(index);
		UInt32List::const_iterator end = it + holeCount + 1; // +1 in order to sum the face-vertex pairs of the polygon as its holes.
		for (; it != end; ++it) count += (*it);
	}
	return count;
}

FCDGeometryPolygonsInput* FCDGeometryPolygons::AddInput(FCDGeometrySource* source, uint32 offset)
{
	FCDGeometryPolygonsInput* input = new FCDGeometryPolygonsInput(GetDocument(), this);
	m_Inputs.push_back(input);
	input->SetOffset(offset);
	input->SetSource(source);
	SetNewChildFlag();
	return input;
}

void FCDGeometryPolygons::SetHoleFaceCount(size_t count)
{
	m_HoleFaces.resize(count);
	SetDirtyFlag();
}

bool FCDGeometryPolygons::IsHoleFaceHole(size_t index)
{
	return m_HoleFaces.find((uint32) index) != m_HoleFaces.end();
}

void FCDGeometryPolygons::AddHole(uint32 index)
{
	FUAssert(!IsHoleFaceHole(index), return);

	// Ordered insert
	const uint32* it = m_HoleFaces.begin();
	for (; it != m_HoleFaces.end(); ++it)
	{
		if (index < (*it)) break;
	}
	m_HoleFaces.insert(it - m_HoleFaces.begin(), index);
}

void FCDGeometryPolygons::AddFaceVertexCount(uint32 count)
{
	m_FaceVertexCounts.push_back(count);
}

void FCDGeometryPolygons::SetFaceVertexCountCount(size_t count)
{
	m_FaceVertexCounts.resize(count);
}

const FCDGeometryPolygonsInput* FCDGeometryPolygons::FindInput(FUDaeGeometryInput::Semantic semantic) const
{
	for (const FCDGeometryPolygonsInput** it = m_Inputs.begin(); it != m_Inputs.end(); ++it)
	{
		if ((*it)->GetSemantic() == semantic) return (*it);
	}
	return nullptr;
}

const FCDGeometryPolygonsInput* FCDGeometryPolygons::FindInput(const FCDGeometrySource* source) const
{
	for (const FCDGeometryPolygonsInput** it = m_Inputs.begin(); it != m_Inputs.end(); ++it)
	{
		if ((*it)->GetSource() == source) return (*it);
	}
	return nullptr;
}

FCDGeometryPolygonsInput* FCDGeometryPolygons::FindInput(const fm::string& sourceId)
{
	const char* s = sourceId.c_str();
	if (*s == '#') ++s;
	size_t inputCount = m_Inputs.size();
	for (size_t i = 0; i < inputCount; ++i)
	{
		FCDGeometryPolygonsInput* input = m_Inputs[i];
		if (input->GetSource()->GetDaeId() == s) return input;
	}
	return nullptr;
}

void FCDGeometryPolygons::FindInputs(FUDaeGeometryInput::Semantic semantic, FCDGeometryPolygonsInputConstList& _inputs) const
{
	for (const FCDGeometryPolygonsInput** it = m_Inputs.begin(); it != m_Inputs.end(); ++it)
	{
		if ((*it)->GetSemantic() == semantic) _inputs.push_back(*it);
	}
}

// Recalculates the face-vertex count within the polygons
void FCDGeometryPolygons::Recalculate()
{
	faceVertexCount = 0;
	for (const uint32* itC = m_FaceVertexCounts.begin(); itC != m_FaceVertexCounts.end(); ++itC)
	{
		faceVertexCount += (*itC);
	}
	SetDirtyFlag();
}

// [DEPRECATED]
bool FCDGeometryPolygons::IsTriangles() const
{
	UInt32List::const_iterator itC;
	for (itC = m_FaceVertexCounts.begin(); itC != m_FaceVertexCounts.end() && (*itC) == 3; ++itC) {}
	return (itC == m_FaceVertexCounts.end());
}

int32 FCDGeometryPolygons::TestPolyType() const
{
	UInt32List::const_iterator itC = m_FaceVertexCounts.begin();
	if (!m_FaceVertexCounts.empty())
	{
		uint32 fCount = *itC;
		for (; itC != m_FaceVertexCounts.end() && *itC == fCount; ++itC) {}
		if (itC == m_FaceVertexCounts.end()) return fCount;
	}
	return -1;
}

// Clone this list of polygons
FCDGeometryPolygons* FCDGeometryPolygons::Clone(FCDGeometryPolygons* clone, const FCDGeometrySourceCloneMap& cloneMap) const
{
	if (clone == nullptr) return nullptr;

	// Clone the miscellaneous information.
	clone->materialSemantic = materialSemantic;
	clone->m_FaceVertexCounts = m_FaceVertexCounts;
	clone->faceOffset = faceOffset;
	clone->faceVertexCount = faceVertexCount;
	clone->faceVertexOffset = faceVertexOffset;
	clone->holeOffset = holeOffset;
	clone->m_HoleFaces = m_HoleFaces;
	
	// Clone the geometry inputs
	// Note that the vertex source inputs are usually created by default.
	size_t inputCount = m_Inputs.size();
	clone->m_Inputs.reserve(inputCount);
	for (size_t i = 0; i < inputCount; ++i)
	{
		// Find the cloned source that correspond to the original input.
		FCDGeometrySource* cloneSource = nullptr;
		FCDGeometrySourceCloneMap::const_iterator it = cloneMap.find(m_Inputs[i]->GetSource());
		if (it == cloneMap.end())
		{
			// Attempt to match by ID instead.
			const fm::string& id = m_Inputs[i]->GetSource()->GetDaeId();
			cloneSource = clone->GetParent()->FindSourceById(id);
		}
		else
		{
			cloneSource = (*it).second;
		}

		// Retrieve or create the input to clone.
		FCDGeometryPolygonsInput* input = clone->FindInput(cloneSource);
		if (input == nullptr)
		{
			input = clone->AddInput(cloneSource, m_Inputs[i]->GetOffset());
		}

		// Clone the input information.
		if (m_Inputs[i]->OwnsIndices())
		{
			input->SetIndices(m_Inputs[i]->GetIndices(), m_Inputs[i]->GetIndexCount());
		}
		input->SetSet(m_Inputs[i]->GetSet());
	}

	return clone;
}
