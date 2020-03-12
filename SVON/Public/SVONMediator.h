#pragma once

namespace SVON {
	class SVONVolume;
	struct SVONLink;
	struct FloatVector;
	struct IntVector;

	class SVONMediator
	{
	public:
		static bool GetLinkFromPosition(const FloatVector& aPositon,
			const SVONVolume& aVolume,
			SVONLink& oLink);

		static void GetVolumeXYZ(const FloatVector& aPosition,
			const SVONVolume& aVolume,
			const int aLayer,
			IntVector& oXYZ);
	};
}