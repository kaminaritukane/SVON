#include "SVONNavigationPath.h"

using namespace SVON;

void SVONNavigationPath::AddPoint(const SVONPathPoint& aPoint)
{
	points.push_back(aPoint);
}
