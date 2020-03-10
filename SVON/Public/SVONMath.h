#pragma once
#include <stdint.h>
#include "SVONAPI.h"

namespace SVON
{
	/**
	 * Structure for integer vectors in 3-d space.
	 */
	struct IntVector
	{
	public:
		int32_t X = 0;
		int32_t Y = 0;
		int32_t Z = 0;

	public:
		__forceinline IntVector(){};

		__forceinline IntVector(int32_t InX, int32_t InY, int32_t InZ)
			: X(InX)
			, Y(InY)
			, Z(InZ)
		{}
	};

	/**
	 * Structure for float vectors in 3-d space.
	 */
	struct FloatVector
	{
	public:
		float X = 0.0f;
		float Y = 0.0f;
		float Z = 0.0f;

	public:
		__forceinline FloatVector(){}

		__forceinline FloatVector(float InX, float InY, float InZ)
			: X(InX)
			, Y(InY)
			, Z(InZ)
		{}

		__forceinline FloatVector(float InF)
			: X(InF)
			, Y(InF)
			, Z(InF)
		{}

		__forceinline FloatVector operator+(const FloatVector& V) const
		{
			return FloatVector(X + V.X, Y + V.Y, Z + V.Z);
		}

		__forceinline FloatVector operator-(const FloatVector& V) const
		{
			return FloatVector(X - V.X, Y - V.Y, Z - V.Z);
		}

		__forceinline FloatVector operator*(const FloatVector& V) const
		{
			return FloatVector(X * V.X, Y * V.Y, Z * V.Z);
		}

		__forceinline FloatVector operator*(float scale) const
		{
			return FloatVector(X * scale, Y * scale, Z * scale);
		}

		__forceinline FloatVector operator/(const FloatVector& V) const
		{
			return FloatVector(X / V.X, Y / V.Y, Z / V.Z);
		}

		__forceinline FloatVector operator/(float scale) const
		{
			const float rScale = 1.f / scale;
			return FloatVector(X * rScale, Y * rScale, Z * rScale);
		}

		__forceinline FloatVector operator+=(const FloatVector& V)
		{
			X += V.X; Y += V.Y; Z += V.Z;
			return *this;
		}

		__forceinline FloatVector operator-=(const FloatVector& V)
		{
			X -= V.X; Y -= V.Y; Z -= V.Z;
			return *this;
		}

		__forceinline FloatVector operator*=(const FloatVector& V)
		{
			X *= V.X; Y *= V.Y; Z *= V.Z;
			return *this;
		}

		__forceinline FloatVector operator*=(float scale)
		{
			X *= scale; Y *= scale; Z *= scale;
			return *this;
		}

		__forceinline FloatVector operator/=(const FloatVector& V)
		{
			X /= V.X; Y /= V.Y; Z /= V.Z;
			return *this;
		}

		__forceinline FloatVector operator/=(float scale)
		{
			const float rScale = 1.f / scale;
			X *= rScale; Y *= rScale; Z *= rScale;
			return *this;
		}
	};

	struct FloatQuaternion
	{
	public:
		float X = 0.0f;
		float Y = 0.0f;
		float Z = 0.0f;
		float W = 0.0f;

	public:
		static const FloatQuaternion Indentity;

	public:
		__forceinline FloatQuaternion() {}

		__forceinline FloatQuaternion(float InX, float InY, float InZ, float InW)
			: X(InX)
			, Y(InY)
			, Z(InZ)
			, W(InW)
		{}
	};
}