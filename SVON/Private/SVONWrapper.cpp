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
	float agentSize,
	intptr_t* pathHandle,
	SVONPathPoint** pathData,
	int* count)
{
	bool ret = false;
	if (vol != nullptr)
	{
		auto volume = *vol;

		SVONLink startNavLink;
		if (!SVONMediator::GetLinkFromPosition(startPos, agentSize, volume, startNavLink))
		{
			// Path finder failed to find start nav link. 
			// Is your gameobject blocking the layer you've selected to generate the nav data with?
			return false;
		}

		SVONLink targetNavLink;
		if (!SVONMediator::GetLinkFromPosition(targetPos, agentSize, volume, targetNavLink))
		{
			// Path finder failed to find target nav link
			return false;
		}

		SVONPathFinderSettings settings;
		SVONPathFinder pathFinder(volume, settings);
		auto pPoints = new std::vector<SVONPathPoint>();

		ret = pathFinder.FindPath(startNavLink, targetNavLink, startPos, targetPos, agentSize, *pPoints);

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

SVON_API bool SVONGetVolumeBlockedBoxes(SVONVolume* vol, intptr_t* boxesHandle, SVONBlockedBox** oBoxes, int* count)
{
	auto volumeBlockBoxes = VolumeBlockBoxes();
	vol->GetVolumeBlockedBoxes(volumeBlockBoxes);

	auto pBlockedBoxes = new std::vector<SVONBlockedBox>();

	for (int i = 0; i < volumeBlockBoxes.size(); ++i)
	{
		auto layerBoxes = volumeBlockBoxes[i];
		float extent = layerBoxes.extent;
		auto boxAmount = layerBoxes.boxCenters.size();
		for (size_t j = 0; j < boxAmount; j++)
		{
			auto box = SVONBlockedBox();
			box.layer = i;
			box.extent = extent;
			box.boxCenter = layerBoxes.boxCenters[j];
			pBlockedBoxes->push_back(box);
		}
	}

	*boxesHandle = reinterpret_cast<intptr_t>(pBlockedBoxes);
	*oBoxes = pBlockedBoxes->data();
	*count = static_cast<int>(pBlockedBoxes->size());

	return true;
}

SVON_API bool ReleaseBoxesHandle(intptr_t boxesHandle)
{
	auto items = reinterpret_cast<SVONBlockedBox*>(boxesHandle);
	delete items;

	return true;
}
