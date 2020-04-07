#pragma once

#include <stdint.h>
#include <vector>
#include "SVONMath.h"

namespace SVON
{
	enum class SVONPathCostType : uint8_t
	{
		MANHATTAN,
		EUCLIDEAN
	};

	struct SVONPathPoint
	{
		FloatVector position; // Potisoin of the point
		int layer; // Layer that the point came from (so we can infer it's volume)
		uint64_t code;

		SVONPathPoint()
			: position(FloatVector())
			, layer(-1)
			, code(0)
		{}

		SVONPathPoint(const FloatVector& aPositoion, int aLayer, uint64_t aCode)
			: position(aPositoion)
			, layer(aLayer)
			, code(aCode)
		{
		}
	};

	//struct SVONNavigationPath
	//{
	//	bool isReady = false;
	//	std::vector<SVONPathPoint> points;

	//	void AddPoint(const SVONPathPoint& aPoint);

	//	bool IsReady() const { return isReady; }
	//	void SetIsReady(bool aIsReady) { isReady = aIsReady; }

	//	
	//};
}