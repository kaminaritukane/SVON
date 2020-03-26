#pragma once

#include "SVONAPI.h"
#include "SVONVolume.h"
#include "SVONNavigationPath.h"

using namespace SVON;

extern "C"
{
	struct SVONNavigationPath;

	SVON_API SVONVolume* CreateSVONVolume(int32_t aVoxelPower,
		GetVolumBoudingBoxFunc getVolumBoudingBoxFunc,
		OverlapBoxBlockingTestFunc boxOverlapCheckFunc);

	SVON_API void ReleaseSVONVolume(SVONVolume* vol);

	SVON_API bool SVONVolumeGenerate(SVONVolume* vol);

	SVON_API bool SVONFindPath(SVONVolume* vol,
		const FloatVector& startPos,
		const FloatVector& targetPos,
		intptr_t* pathHandle,
		SVONPathPoint** pathData,
		int* count);

	SVON_API bool ReleasePathHandle(intptr_t pathHandle);

	SVON_API bool SVONGetVolumeBlockedBoxes(SVONVolume* vol,
		intptr_t* boxesHandle,
		SVONBlockedBoxes** oBoxes,
		int* count);

	SVON_API bool ReleaseBoxesHandle(intptr_t boxesHandle);
}