#pragma once

#include <vector>
#include "SVONDefines.h"
#include "SVONNode.h"
#include "SVONLeafNode.h"

namespace SVON
{
	struct SVONData
	{
		std::vector<std::vector<SVONNode>> layers;
		std::vector<SVONLeafNode> leafNodes;

		void Reset()
		{
			layers.clear();
			leafNodes.clear();
		}

		int GetSize()
		{
			size_t result = 0;
			result += leafNodes.size() * sizeof(SVONLeafNode);
			for (size_t i = 0; i < layers.size(); ++i)
			{
				result += layers[i].size() * sizeof(SVONNode);
			}

			return static_cast<int>(result);
		}
	};
}