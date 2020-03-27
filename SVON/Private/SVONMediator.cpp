#include "SVONMediator.h"
#include "SVONVolume.h"

using namespace SVON;

bool SVONMediator::GetLinkFromPosition(const FloatVector& aPositon, 
	float agentSize,
	const SVONVolume& aVolume, 
	SVONLink& oLink)
{
	if (!aVolume.EncomppassesPoint(aPositon))
	{
		return false;
	}

	int layerIndex = aVolume.GetNumLayers() - 1;
	nodeindex_t nodeIndex = 0;

	int lastLayerIndex = 0;
	nodeindex_t lastNodeIndex = 0;

	while (layerIndex >= 0
		&& layerIndex < aVolume.GetNumLayers())
	{
		auto voxelSize = aVolume.GetVoxelSize(layerIndex);
		if (voxelSize < agentSize)
		{
			// Get parent node
			oLink.layerIndex = lastLayerIndex;
			oLink.nodeIndex = lastNodeIndex;
			oLink.subnodeIndex = 0;
			return true;
		}

		// Get the layer and voxel size
		const std::vector<SVONNode>& layer = aVolume.GetLayer(layerIndex);

		// Calculate the XYZ coordinates
		IntVector voxel;
		GetVolumeXYZ(aPositon, aVolume, layerIndex, voxel);
		uint_fast32_t x, y, z;
		x = voxel.X;
		y = voxel.Y;
		z = voxel.Z;

		// Get the morton code we want for this layer
		mortoncode_t code = morton3D_64_encode(x, y, z);
		// If a node has 8 child nodes, then max morton code is 7
		int32_t maxCode = aVolume.GetNodesInLayer(layerIndex) - 1;
		if (code > maxCode)
		{
			// the position is right on the volume edge, 
			// should enlarge the volume a little in GetVolumBoudingBoxFunc callback
			return false;
		}

		for (nodeindex_t j = nodeIndex; j < layer.size(); ++j)
		{
			const SVONNode& node = layer[j];
			// This is the node we are in
			if (node.code == code)
			{
				// There are no child nodes, so this is our nav position
				if (!node.firstChild.IsValid())
				{
					oLink.layerIndex = layerIndex;
					oLink.nodeIndex = j;
					oLink.subnodeIndex = 0;
					return true;
				}

				// If this is a leaf node, we need to find our subnode
				if (layerIndex == 0)
				{
					float voxelSize = aVolume.GetVoxelSize(layerIndex);
					float subnodeVoxelSize = voxelSize * 0.25f;
					if (subnodeVoxelSize < agentSize)
					{
						// TODO: check whether agent overlaps with subnodes
						// for now, return false. means the agent is overlaps with this layer 0 node
						return false;
					}

					const SVONLeafNode& leaf = aVolume.GetLeafNode(node.firstChild.nodeIndex);
					// We need to calculate the node local positon to get the morton code for the leaf
					// The world positon of the 0 node
					FloatVector nodePosition;
					aVolume.GetNodePosition(layerIndex, node.code, nodePosition);
					// The morton origin of the node
					FloatVector nodeOrigin = nodePosition - FloatVector(voxelSize * 0.5f);
					// The requested positon, lelative to the node orign
					FloatVector nodeLocalPos = aPositon - nodeOrigin;
					// Now get our voxel coordinates
					IntVector coord;
					coord.X = static_cast<int>(round(nodeLocalPos.X / subnodeVoxelSize + 0.5));
					coord.Y = static_cast<int>(round(nodeLocalPos.Y / subnodeVoxelSize + 0.5));
					coord.Z = static_cast<int>(round(nodeLocalPos.Z / subnodeVoxelSize + 0.5));

					// So our link is.....*drum roll*
					oLink.layerIndex = 0; // Layer 0 (leaf)
					oLink.nodeIndex = j; // this index

					// This morton code is our key into the 64-bit leaf node
					mortoncode_t leafIndex = morton3D_64_encode(coord.X, coord.Y, coord.Z);

					if (leaf.GetNode(leafIndex))
					{
						return false;// This voxel is blocked, oops!
					}

					oLink.subnodeIndex = leafIndex;

					return true;
				}

				lastLayerIndex = layerIndex;
				lastNodeIndex = j;

				// If we've got here, the current node has a child, and isn't a leaf
				// so lets go down...
				layerIndex = layer[j].firstChild.GetLayerIndex();
				nodeIndex = layer[j].firstChild.GetNodeIndex();

				break; // stop iterating this layer
			}
		}
	}

	return false;
}

void SVONMediator::GetVolumeXYZ(const FloatVector& aPosition, 
	const SVONVolume& aVolume, 
	const int aLayer, 
	IntVector& oXYZ)
{
	const FloatVector& volumeOrigin = aVolume.GetOrigin();
	const FloatVector& volumeExtent = aVolume.GetExtent();
	// The z-order origin of the volume (where code == 0)
	auto zOrigin = volumeOrigin - volumeExtent;
	// The local positon of the point in volume space
	auto localPos = aPosition - zOrigin;

	int layerIndex = aLayer;

	// Get the layer and voxel size
	float voxelSize = aVolume.GetVoxelSize(layerIndex);

	// Calculate the XYZ coordinates
	oXYZ.X = static_cast<int>(localPos.X / voxelSize);
	oXYZ.Y = static_cast<int>(localPos.Y / voxelSize);
	oXYZ.Z = static_cast<int>(localPos.Z / voxelSize);
}
