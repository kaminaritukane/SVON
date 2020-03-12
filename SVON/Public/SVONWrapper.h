#pragma once

#include "SVONAPI.h"
#include "SVONVolume.h"
#include "SVONNavigationPath.h"

namespace SVON
{
	struct SVONNavigationPath;

	class SVON_API SVONWrapper
	{
	public:
		static SVONWrapper* GetInstance()
		{
			if (instance == nullptr)
			{
				instance = new SVONWrapper();
			}
			return instance;
		}

		SVONVolume* CreateSVONVolume(int32_t aVoxelPower,
			GetVolumBoudingBoxFunc getVolumBoudingBoxFunc,
			OverlapBoxBlockingTestFunc boxOverlapCheckFunc);

		void ReleaseSVONVolume(SVONVolume* vol);

		bool SVONVolumeGenerate(SVONVolume* vol);

		bool SVONFindPath(SVONVolume* vol,
			const FloatVector& startPos,
			const FloatVector& targetPos,
			SVONNavigationPath& oPath);

	private:
		static SVONWrapper* instance;
	};
}
