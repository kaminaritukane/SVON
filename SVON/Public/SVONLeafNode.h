#pragma once
#include "SVONDefines.h"
#include "SVONLink.h"
#include "libmorton/morton.h"

namespace SVON
{
	struct SVONLeafNode
	{
		uint_fast64_t voxelGrid = 0;

		inline bool GetNodeAt(uint_fast32_t ax, uint_fast32_t ay, uint_fast32_t az) const
		{
			uint_fast64_t index = morton3D_64_encode(ax, ay, az);
			return (voxelGrid & (1ULL << index)) != 0;
		}

		inline void SetNodeAt(uint_fast32_t ax, uint_fast32_t ay, uint_fast32_t az)
		{
			uint_fast64_t index = morton3D_64_encode(ax, ay, az);
			voxelGrid |= 1ULL << index;
		}

		inline bool GetNode(mortoncode_t aIndex) const
		{
			return (voxelGrid & (1ULL << aIndex)) != 0;
		}

		inline void SetNode(uint8_t aIndex)
		{
			voxelGrid |= 1ULL << aIndex;
		}

		inline bool IsCompletelyBlocked() const
		{
			return voxelGrid == -1;
		}

		inline bool IsEmpty() const
		{
			return voxelGrid == 0;
		}
	};
}