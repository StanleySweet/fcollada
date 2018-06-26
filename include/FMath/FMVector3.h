/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America

	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FMVector3.h The file containing the class and global functions for 3 dimensional vectors.
*/

#include <algorithm>

#ifndef _FM_VECTOR3_H_
#define _FM_VECTOR3_H_

class FMVector4;

/**
	A 3 dimensional vector.

	Simple, non-optimized vector class: * is the dot-product, ^ is the
	cross-product.

	@ingroup FMath
*/
class FCOLLADA_EXPORT
	ALIGN_STRUCT(16)
	FMVector3
{
public:
	float m_X;	/**< The first coordinate. */
	float m_Y;	/**< The second coordinate. */
	float m_Z;	/**< The third coordinate. */
private:
	float m_W;	// For alignment purposes.

public:
	/**
	 * Creates an empty FMVector3.
	 */
#ifndef _DEBUG
	inline FMVector3() {}
#else
	inline FMVector3() { m_X = 123456789.0F; m_Y = 123456789.0F; m_Z = 123456789.0F; }
#endif 

	/** Creates the FMVector3 with the coordinates given.
		@param _x The first coordinate.
		@param _y The second coordinate.
		@param _z The third coordinate. */
	inline FMVector3(float _x, float _y, float _z) { m_X = _x; m_Y = _y; m_Z = _z; }

	/** Copy constuctor.
		@param rhs A second 3D vector. */
	inline FMVector3(const FMVector3& rhs) { m_X = rhs.m_X; m_Y = rhs.m_Y; m_Z = rhs.m_Z; }

	/** Constructor.
		Reduces the 4D vector into 3D by removing the 4th dimension.
		@param vect4 A 4D vector. */
	FMVector3(const FMVector4& vect4);

	/** Creates the FMVector3 from a list of floating-point values.
		It takes the first three floating-point starting from and including \a startIndex
		(0 indexing) in the array as the 3 coordinates. The first as the first
		coordinate, the second as the second, and the third as the third.
		@param source The floating-point value array.
		@param startIndex The index of the first element. */
	FMVector3(const float* source, uint32 startIndex = 0);
	FMVector3(const double* source, uint32 startIndex = 0);	/**< See above. */

	/**	Retrieves the squared length of the vector.
		@return The squared length of this vector. */
	inline float LengthSquared() const { return m_X * m_X + m_Y * m_Y + m_Z * m_Z; }

	/**	Retrieves the length of the vector.
		@return The length of this vector. */
	inline float Length() const { return sqrtf(m_X * m_X + m_Y * m_Y + m_Z * m_Z); }

	/** Normalizes this vector. */
	inline void NormalizeIt() { float l = Length(); if (l > 0.0F) { m_X /= l; m_Y /= l; m_Z /= l; } else { m_X = m_Y = 0; m_Z = 1; } }

	/** Get a normalized FMVector3 with the same direction as this vector.
		@return A FMVector3 with length 1 and same direction as this vector. */
	inline FMVector3 Normalize() const { float l = Length(); return (l > 0.0F) ? FMVector3(m_X / l, m_Y / l, m_Z / l) : FMVector3::XAxis; }

	/** Project this FMVector3 onto another FMVector3.
		@param unto The FMVector3 to project onto. */
	inline void Project(const FMVector3& unto) { (*this) = Projected(unto); }

	/** Get the projection of this FMVector3 onto another FMVector3.
		@param unto The FMVector3 to project onto.
		@return The projected FMVector3. */
	inline FMVector3 Projected(const FMVector3& unto) const;

	/** Get this FMVector3 as an array of \c floats.
		@return The \c float array. */
	inline operator float*() { return &m_X; }

	/** Get this FMVector3 as an array of \c floats.
		@return The \c float array. */
	inline operator const float*() const { return &m_X; }

	/** Set the values of this vector
		@param x The new X value
		@param y The new Y value
		@param z The new Z value */
	inline void Set(float x, float y, float z) { m_X = x; m_Y = y, m_Z = z; }

	/** Assign this FMVector3 to the given float array.
		Assigns each coordinate of this FMVector3 to the elements in the \c
		float array. The first element to the first coordinate, the second to
		the second, and the third to the third. It returns this FMVector3.
		@param v The \c float array to assign with.
		@return This vector. */
	inline FMVector3& operator =(const float* v) { m_X = *v; m_Y = *(v + 1); m_Z = *(v + 2); return *this; }

	/** Assigns the FMVector3 passed to outselves.
		Copies XYZ from the passed vector
		@param rhs The vector copy off */
	inline FMVector3& operator =(const FMVector3& rhs) { m_X = rhs.m_X; m_Y = rhs.m_Y; m_Z = rhs.m_Z; return *this; }

	/** Update each component of this FMVector to the minimum of two FMVector3s.
		Updates each of the three components to be the minimum of the current
		value and that of the corresponding value of the given FMVector3.
		@param min The FMVector to take values from. */
	inline void ComponentMinimum(const FMVector3& min) { if (m_X < min.m_X) m_X = min.m_X; if (m_Y < min.m_Y) m_Y = min.m_Y; if (m_Z < min.m_Z) m_Z = min.m_Z; }

	/** Retrieves the smallest component of the vector.
		@return The smallest component of the vector. */
	inline float ComponentMinimum() const { return std::min(fabsf(m_X), std::min(fabsf(m_Y), fabsf(m_Z))); }

	/** Update each component of this FMVector to the maximum of two FMVector3s.
		Updates each of the three components to be the maximum of the current
		value and that of the corresponding value of the given FMVector3.
		@param max The FMVector to take values from. */
	inline void ComponentMaximum(const FMVector3& max) { if (m_X > max.m_X) m_X = max.m_X; if (m_Y > max.m_Y) m_Y = max.m_Y; if (m_Z > max.m_Z) m_Z = max.m_Z; }

	/** Retrieves the largest component of the vector.
		@return The largest component of the vector. */
	inline float ComponentMaximum() const { return std::max(fabsf(m_X), std::max(fabsf(m_Y), fabsf(m_Z))); }

	/** Clamp each component of this FMVector by the corresponding components
		in the specified min and max FMVector3.
		Clamp refers to setting a value within a given range. If the value is
		lower than the minimum of the range, it is set to the minimum; same for
		the maximum.
		@param min The FMVector to take the minimum values from.
		@param max The FMVector to take the maximum values from. */
	inline void ComponentClamp(const FMVector3& min, const FMVector3& max) { ComponentMinimum(min); ComponentMaximum(max); }

	/** Retrieves the average of the three vector components.
		@return The component average. */
	inline float ComponentAverage() const { return (fabsf(m_X) + fabsf(m_Y) + fabsf(m_Z)) / 3.0F; }

public:
	static const FMVector3 XAxis; /**< The FMVector3 representing the x axis */
	static const FMVector3 YAxis; /**< The FMVector3 representing the y axis */
	static const FMVector3 ZAxis; /**< The FMVector3 representing the z axis */
	static const FMVector3 Origin;/**< The FMVector3 representing the origin */
	static const FMVector3 Zero;  /**< The FMVector3 containing all zeroes: (0,0,0). */
	static const FMVector3 One;	  /**< The FMVector3 containing all ones: (1,1,1). */
};

/** Vector addition with two FMVector3.
	@param a The first vector.
	@param b The second vector.
	@return The FMVector3 representation of the resulting vector. */
inline FMVector3 operator +(const FMVector3& a, const FMVector3& b) { return FMVector3(a.m_X + b.m_X, a.m_Y + b.m_Y, a.m_Z+ b.m_Z); }

/** Vector subtraction with two FMVector3.
	@param a The first vector.
	@param b The second vector.
	@return The FMVector3 representation of the resulting vector. */
inline FMVector3 operator -(const FMVector3& a, const FMVector3& b) { return FMVector3(a.m_X - b.m_X, a.m_Y - b.m_Y, a.m_Z - b.m_Z); }

/** Positive operator of the given FMVector3.
	It applies the positive operator to each of the components of the FMVector3.
	@param a The vector to apply the positive operator to.
	@return The FMVector3 representation of the resulting vector. */
inline FMVector3 operator +(const FMVector3& a) { return FMVector3(+a.m_X, +a.m_Y, +a.m_Z); }

/** Negates the given FMVector3.
	It negates each of the components of the FMVector3.
	@param a The vector to negate.
	@return The FMVector3 representation of the resulting vector. */
inline FMVector3 operator -(const FMVector3& a) { return FMVector3(-a.m_X, -a.m_Y, -a.m_Z); }

/** Dot product of two FMVector3.
	@param a The first vector.
	@param b The second vector.
	@return The result of the dot product. */
inline float operator *(const FMVector3& a, const FMVector3& b) { return a.m_X * b.m_X + a.m_Y * b.m_Y + a.m_Z * b.m_Z; }

/** Scalar multiplication with a FMVector3.
	@param a The vector.
	@param b The scalar.
	@return The FMVector3 representing the resulting vector. */
inline FMVector3 operator *(const FMVector3& a, float b) { return FMVector3(a.m_X * b, a.m_Y * b, a.m_Z * b); }

/** Scalar multiplication with a FMVector3.
	@param a The scalar.
	@param b The vector.
	@return The FMVector3 representing the resulting vector. */
inline FMVector3 operator *(float a, const FMVector3& b) { return FMVector3(a * b.m_X, a * b.m_Y, a * b.m_Z); }

/** Scalar division with a FMVector3.
	@param a The vector.
	@param b The scalar.
	@return The FMVector3 representing the resulting vector. */
inline FMVector3 operator /(const FMVector3& a, float b) { return FMVector3(a.m_X / b, a.m_Y / b, a.m_Z / b); }

/** Cross product of two FMVector3.
	@param a The first vector.
	@param b The second vector.
	@return The result of the dot product. */
inline FMVector3 operator ^(const FMVector3& a, const FMVector3& b) { return FMVector3(a.m_Y * b.m_Z - a.m_Z * b.m_Y, a.m_Z * b.m_X - a.m_X * b.m_Z, a.m_X * b.m_Y - a.m_Y * b.m_X); }

/** Assignment of the addition of two FMVector3.
	@param b The first vector, which will also be assigned to the result.
	@param a The second vector.
	@return The first vector, after it has been assigned new values. */
inline FMVector3& operator +=(FMVector3& b, const FMVector3& a) { b.m_X += a.m_X; b.m_Y += a.m_Y; b.m_Z += a.m_Z; return b; }

/**	Assignment of the subtraction of two FMVector3.
	@param b The first vector, which will also be assigned to the result.
	@param a The second vector.
	@return The first vector, after it has been assigned new values. */
inline FMVector3& operator -=(FMVector3& b, const FMVector3& a) { b.m_X -= a.m_X; b.m_Y -= a.m_Y; b.m_Z -= a.m_Z; return b; }

/** Assignment of the scalar multiplication of a FMVector3.
	@param b The vector, which will also be assigned to the result.
	@param a The scalar.
	@return The vector, after it has been assigned new values. */
inline FMVector3& operator *=(FMVector3& b, float a) { b.m_X *= a; b.m_Y *= a; b.m_Z *= a; return b; }

/** Assignment of the scalar division of a FMVector3.
	@param b The vector, which will also be assigned to the result.
	@param a The scalar.
	@return The vector, after it has been assigned new values. */
inline FMVector3& operator /=(FMVector3& b, float a) { b.m_X /= a; b.m_Y /= a; b.m_Z /= a; return b; }

/** Returns whether two 3D vectors or points are equivalent.
	@param p A first vector.
	@param q A second vector.
	@return Whether the vectors are equivalent. */
inline bool IsEquivalent(const FMVector3& p, const FMVector3& q) { return IsEquivalent(p.m_X, q.m_X) && IsEquivalent(p.m_Y, q.m_Y) && IsEquivalent(p.m_Z, q.m_Z); }
inline bool operator == (const FMVector3& p, const FMVector3& q) { return IsEquivalent(p, q); } /**< See above. */

// Already documented above.
inline FMVector3 FMVector3::Projected(const FMVector3& unto) const { return ((*this) * unto) / unto.LengthSquared() * unto; }

/** A dynamically-sized array of 3D vectors or points. */
typedef fm::vector<FMVector3> FMVector3List;

#endif // _FM_VECTOR3_H_
