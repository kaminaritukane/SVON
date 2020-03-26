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
	intptr_t* pathHandle,
	SVONPathPoint** pathData,
	int* count)
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
		auto pPoints = new std::vector<SVONPathPoint>();

		ret = pathFinder.FindPath(startNavLink, targetNavLink, startPos, targetPos, *pPoints);

		*pathHandle = reinterpret_cast<intptr_t>(pPoints);
		*pathData = pPoints->data();
		*count = static_cast<int>(pPoints->size());
	}

	return ret;
}

SVON_API bool ReleasePathHandle(intptr_t pathHandle)
{
	auto items = reinterpret_cast<std::vector<SVONPathPoint>*>(pathHandle);
	delete items;

	return true;
}

SVON_API bool SVONGetVolumeBlockedBoxes(SVONVolume* vol, intptr_t* boxesHandle, SVONBlockedBoxes** oBoxes, int* count)
{
	auto pVolumeBlockBoxes = new VolumeBlockBoxes();
	vol->GetVolumeBlockedBoxes(*pVolumeBlockBoxes);

	*boxesHandle = reinterpret_cast<intptr_t>(pVolumeBlockBoxes);
	*oBoxes = pVolumeBlockBoxes->data();
	*count = static_cast<int>(pVolumeBlockBoxes->size());

	return true;
}

SVON_API bool ReleaseBoxesHandle(intptr_t boxesHandle)
{
	auto items = reinterpret_cast<VolumeBlockBoxes*>(boxesHandle);
	delete items;

	return true;
}
