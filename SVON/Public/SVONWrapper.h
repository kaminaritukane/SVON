#pragma once

#include "SVONAPI.h"
#include "SVONVolume.h"
#include "SVONNavigationPath.h"

using namespace SVON;

extern "C"
{
	struct SVONNavigationPath;

	struct SVONVoxelBox {
		short layer = 0;
		bool blocked = false;
		float extent = 0.0f;
		mortoncode_t mortonCode = 0;
		FloatVector boxCenter;
	};

	SVON_API SVONVolume* CreateSVONVolume(int32_t aVoxelPower,
		GetVolumBoudingBoxFunc getVolumBoudingBoxFunc,
		OverlapBoxBlockingTestFunc boxOverlapCheckFunc);

	SVON_API void ReleaseSVONVolume(SVONVolume* vol);

	SVON_API bool SVONVolumeGenerate(SVONVolume* vol);

	SVON_API bool SVONFindPath(SVONVolume* vol,
		const FloatVector& startPos,
		const FloatVector& targetPos,
		float agentSize,
		intptr_t* pathHandle,
		SVONPathPoint** pathData,
		int* count);

	SVON_API bool ReleasePathHandle(intptr_t pathHandle);

	SVON_API bool SVONGetVolumeVoxelBoxes(SVONVolume* vol,
		intptr_t* boxesHandle,
		SVONVoxelBox** oBoxes,
		int* count);

	SVON_API bool ReleaseBoxesHandle(intptr_t boxesHandle);
}