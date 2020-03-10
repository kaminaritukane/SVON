#include "..\Public\SVONWrapper.h"
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
