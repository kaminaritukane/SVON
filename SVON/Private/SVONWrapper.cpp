#include "SVONWrapper.h"
#include "SVONPathFinder.h"
#include "SVONMediator.h"

SVONVolume* CreateSVONVolume(int32_t aVoxelPower,
	GetVolumBoudingBoxFunc getVolumBoudingBoxFunc, 
	OverlapBoxBlockingTestFunc boxOverlapCheckFunc)
{
	return new SVONVolume(aVoxelPower, getVolumBoudingBoxFunc, boxOverlapCheckFunc);
}


void ReleaseSVONVolume(SVONVolume* vol)
{
	delete vol;
}

bool SVONVolumeGenerate(SVONVolume* vol)
{
	bool ret = false;
	if (vol != nullptr)
	{
		ret = vol->Generate();
	}
	return ret;
}

bool SVONFindPath(SVONVolume* vol,
	const FloatVector& startPos,
	const FloatVector& targetPos, 
	SVON::SVONNavigationPath& oPath)
{
	bool ret = false;
	if (vol != nullptr)
	{
		auto volume = *vol;

		SVONLink startNavLink;
		if (!SVONMediator::GetLinkFromPosition(startPos, volume, startNavLink))
		{
			// Path finder failed to find start nav link. 
			// Is your gameobject blocking the layer you've selected to generate the nav data with?
			return false;
		}

		SVONLink targetNavLink;
		if (!SVONMediator::GetLinkFromPosition(targetPos, volume, targetNavLink))
		{
			// Path finder failed to find target nav link
			return false;
		}

		SVONPathFinderSettings settings;
		SVONPathFinder pathFinder(volume, settings);

		ret = pathFinder.FindPath(startNavLink, targetNavLink, startPos, targetPos, oPath);
	}

	return ret;
}

