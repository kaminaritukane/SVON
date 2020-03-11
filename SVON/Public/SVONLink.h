#pragma once

#include <stdint.h>
#include "SVONString.h"

namespace SVON
{
	struct SVONLink
	{
		uint32_t layerIndex : 4; // 0~15
		uint32_t nodeIndex : 22; // 0~4194303
		uint32_t subnodeIndex : 6; // 0~63

		SVONLink()
			: layerIndex(15)
			, nodeIndex(0)
			, subnodeIndex(0)
		{}

		SVONLink(uint8_t alayer, uint_fast32_t aNodeIndex, uint8_t aSubNodeIndex)
			: layerIndex(alayer)
			, nodeIndex(aNodeIndex)
			, subnodeIndex(aSubNodeIndex)
		{}

		uint8_t GetLayerIndex() const { return layerIndex; }
		void SetLayerIndex(const uint8_t aLayerIndex) { layerIndex = aLayerIndex; }

		uint_fast32_t GetNodeIndex() const { return nodeIndex; }
		void SetNodeIndex(const uint_fast32_t aNodeIndex) { nodeIndex = aNodeIndex; }

		uint8_t GetSubnodeIndex() const { return subnodeIndex; }
		void SetSubnodeIndex(const uint8_t aSubnodeIndex) { subnodeIndex = aSubnodeIndex; }

		bool IsValid() const { return layerIndex != 15; }
		void SetInvalid() { layerIndex = 15; }

		bool operator==(const SVONLink& aOther) const {
			return memcmp(this, &aOther, sizeof(SVONLink)) == 0;
		}

		bool operator!=(const SVONLink& aOther) const {
			return memcmp(this, &aOther, sizeof(SVONLink)) != 0;
		}

		bool operator<(const SVONLink& aOther) const {
			return memcmp(this, &aOther, sizeof(SVONLink)) < 0;
		}

		bool operator>(const SVONLink& aOther) const {
			return memcmp(this, &aOther, sizeof(SVONLink)) > 0;
		}

		static SVONLink GetInvalidLink() { return SVONLink(15, 0, 0); }

		std::string ToString()
		{
			return formatString("%i:%i:%i", layerIndex, nodeIndex, subnodeIndex);
		}
	};
}