/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America

	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FUBoundingBox.h"
#include "FUBoundingSphere.h"

#define FUBOUNDINGBOX_COORDINATE_COUNT 8
#define FUBOUNDINGBOX_COORDINATES(coordinates, boundingBox) \
	FMVector3 coordinates[FUBOUNDINGBOX_COORDINATE_COUNT] = { \
		boundingBox.GetMin(), \
		FMVector3(boundingBox.GetMin().x, boundingBox.GetMin().y, boundingBox.GetMax().z), \
		FMVector3(boundingBox.GetMin().x, boundingBox.GetMax().y, boundingBox.GetMin().z), \
		FMVector3(boundingBox.GetMax().x, boundingBox.GetMin().y, boundingBox.GetMin().z), \
		FMVector3(boundingBox.GetMin().x, boundingBox.GetMax().y, boundingBox.GetMax().z), \
		FMVector3(boundingBox.GetMax().x, boundingBox.GetMin().y, boundingBox.GetMax().z), \
		FMVector3(boundingBox.GetMax().x, boundingBox.GetMax().y, boundingBox.GetMin().z), \
		boundingBox.GetMax() }

//
// FUBoundingBox
//

// static variables initialization
const FUBoundingBox FUBoundingBox::Infinity(FMVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX), FMVector3(FLT_MAX, FLT_MAX, FLT_MAX));

FUBoundingBox::FUBoundingBox()
:	minimum(FLT_MAX, FLT_MAX, FLT_MAX)
,	maximum(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
}

FUBoundingBox::FUBoundingBox(const FMVector3& _min, const FMVector3& _max)
:	minimum(_min), maximum(_max)
{
}

FUBoundingBox::FUBoundingBox(const FUBoundingBox& copy)
:	minimum(copy.minimum), maximum(copy.maximum)
{
}

FUBoundingBox::~FUBoundingBox()
{
}

void FUBoundingBox::Reset()
{
	minimum = FMVector3(FLT_MAX, FLT_MAX, FLT_MAX);
	maximum = FMVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

bool FUBoundingBox::IsValid() const
{
	return !(minimum.m_X > maximum.m_X || minimum.m_Y > maximum.m_Y || minimum.m_Z > maximum.m_Z);
}

bool FUBoundingBox::Contains(const FMVector3& point) const
{
	return minimum.m_X <= point.m_X && point.m_X <= maximum.m_X
		&& minimum.m_Y <= point.m_Y && point.m_Y <= maximum.m_Y
		&& minimum.m_Z <= point.m_Z && point.m_Z <= maximum.m_Z;
}

bool FUBoundingBox::Overlaps(const FUBoundingBox& boundingBox, FMVector3* overlapCenter) const
{
	bool overlaps = minimum.m_X <= boundingBox.maximum.m_X && boundingBox.minimum.m_X <= maximum.m_X
		&& minimum.m_Y <= boundingBox.maximum.m_Y && boundingBox.minimum.m_Y <= maximum.m_Y
		&& minimum.m_Z <= boundingBox.maximum.m_Z && boundingBox.minimum.m_Z <= maximum.m_Z;
	if (overlaps && overlapCenter != NULL)
	{
		float overlapMinX = max(minimum.m_X, boundingBox.minimum.m_X);
		float overlapMaxX = min(maximum.m_X, boundingBox.maximum.m_X);
		float overlapMinY = max(minimum.m_Y, boundingBox.minimum.m_Y);
		float overlapMaxY = min(maximum.m_Y, boundingBox.maximum.m_Y);
		float overlapMinZ = max(minimum.m_Z, boundingBox.minimum.m_Z);
		float overlapMaxZ = min(maximum.m_Z, boundingBox.maximum.m_Z);
		(*overlapCenter) = FMVector3((overlapMaxX + overlapMinX) / 2.0f, (overlapMaxY + overlapMinY) / 2.0f, (overlapMaxZ + overlapMinZ) / 2.0f);
	}
	return overlaps;
}

bool FUBoundingBox::Overlaps(const FUBoundingSphere& boundingSphere, FMVector3* overlapCenter) const
{
	// already implemented in bounding sphere code.
	return boundingSphere.Overlaps(*this, overlapCenter);
}

void FUBoundingBox::Include(const FUBoundingBox& boundingBox)
{
	 const FMVector3& n = boundingBox.minimum;
	 const FMVector3& x = boundingBox.maximum;
	 if (n.m_X < minimum.m_X) minimum.m_X = n.m_X;
	 if (n.m_Y < minimum.m_Y) minimum.m_Y = n.m_Y;
	 if (n.m_Z < minimum.m_Z) minimum.m_Z = n.m_Z;
	 if (x.m_X > maximum.m_X) maximum.m_X = x.m_X;
	 if (x.m_Y > maximum.m_Y) maximum.m_Y = x.m_Y;
	 if (x.m_Z > maximum.m_Z) maximum.m_Z = x.m_Z;
}

void FUBoundingBox::Include(const FMVector3& point)
{
	if (point.m_X < minimum.m_X) minimum.m_X = point.m_X;
	else if (point.m_X > maximum.m_X) maximum.m_X = point.m_X;
	if (point.m_Y < minimum.m_Y) minimum.m_Y = point.m_Y;
	else if (point.m_Y > maximum.m_Y) maximum.m_Y = point.m_Y;
	if (point.m_Z < minimum.m_Z) minimum.m_Z = point.m_Z;
	else if (point.m_Z > maximum.m_Z) maximum.m_Z = point.m_Z;
}

FUBoundingBox FUBoundingBox::Transform(const FMMatrix44& transform) const
{
	if (!IsValid() || Equals(Infinity)) return (*this);

	FUBoundingBox transformedBoundingBox;

	FMVector3 testPoints[6] =
	{
		FMVector3(minimum.m_X, maximum.m_Y, minimum.m_Z), FMVector3(minimum.m_X, maximum.m_Y, maximum.m_Z),
		FMVector3(maximum.m_X, maximum.m_Y, minimum.m_Z), FMVector3(minimum.m_X, minimum.m_Y, maximum.m_Z),
		FMVector3(maximum.m_X, minimum.m_Y, minimum.m_Z), FMVector3(maximum.m_X, minimum.m_Y, maximum.m_Z)
	};

	for (size_t i = 0; i < 6; ++i)
	{
		testPoints[i] = transform.TransformCoordinate(testPoints[i]);
		transformedBoundingBox.Include(testPoints[i]);
	}
	transformedBoundingBox.Include(transform.TransformCoordinate(minimum));
	transformedBoundingBox.Include(transform.TransformCoordinate(maximum));

	return transformedBoundingBox;
}

bool FUBoundingBox::Equals(const FUBoundingBox& right) const
{
	return (minimum.m_X == right.minimum.m_X &&
		maximum.m_X == right.maximum.m_X &&
		minimum.m_Y == right.minimum.m_Y &&
		maximum.m_Y == right.maximum.m_Y &&
		minimum.m_Z == right.minimum.m_Z &&
		maximum.m_Z == right.maximum.m_Z);
}
