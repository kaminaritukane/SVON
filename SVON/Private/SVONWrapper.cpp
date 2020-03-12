#include "SVONWrapper.h"
#include "SVONPathFinder.h"

using namespace SVON;

SVONWrapper* SVONWrapper::instance = nullptr;

SVONVolume* SVONWrapper::CreateSVONVolume(int32_t aVoxelPower,
	GetVolumBoudingBoxFunc getVolumBoudingBoxFunc,
	OverlapBoxBlockingTestFunc boxOverlapCheckFunc)
{
	return new SVONVolume(aVoxelPower, getVolumBoudingBoxFunc, boxOverlapCheckFunc);
}

void SVONWrapper::ReleaseSVONVolume(SVONVolume* vol)
{
	delete vol;
}

bool SVONWrapper::SVONVolumeGenerate(SVONVolume* vol)
{
	bool ret = false;
	if (vol != nullptr)
	{
		ret = vol->Generate();
	}
	return ret;
}

bool SVONWrapper::SVONFindPath(SVONVolume* vol,
	const FloatVector& startPos,
	const FloatVector& targetPos, 
	SVONNavigationPath& oPath)
{
	bool ret = false;
	if (vol != nullptr)
	{
		auto volume = *vol;
		SVONPathFinder pathFinder(volume, SVONPathFinderSettings());

		//volume.GetLeafNeighbours

		//bool ret = pathFinder.FindPath()
	}
	return ret;
}
