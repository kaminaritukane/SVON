#include "SVONVolume.h"
#include <queue>
#include <assert.h>

using namespace SVON;

bool SVONVolume::Generate()
{
	SetupVolume();

	blockedIndices.clear();
	data.layers.clear();

	numLayers = voxelPower + 1;

	// Rasterize at Layer 1
	FirstPassRasterize();

	data.leafNodes.clear();
	data.leafNodes.assign(static_cast<size_t>(blockedIndices[0].size() * 8.0f * 0.25f), SVONLeafNode());

	// Add layers
	for (int i = 0; i < numLayers; ++i)
	{
		data.layers.push_back(std::vector<SVONNode>());
	}

	// Rasterize layer, botton up, adding parent/child links
	for (int i = 0; i < numLayers; ++i)
	{
		RasterizeLayer(i);
	}

	// Now traverse down, adding neighbour links
	for (int i = numLayers - 2; i >= 0; --i)
	{
		BuildNeighbourLinks(i);
	}

	numBytes = data.GetSize();

	return true;
}

std::vector<SVONNode>& SVONVolume::GetLayer(layerindex_t aLayer)
{
	return data.layers[aLayer];
}


const std::vector<SVONNode>& SVONVolume::GetLayer(layerindex_t aLayer) const
{
	return data.layers[aLayer];
}

float SVONVolume::GetVoxelSize(layerindex_t aLayer) const
{
	return (extent.X / powf(2, static_cast<float>(voxelPower))) * (powf(2.0f, static_cast<float>(aLayer + 1)));
}

bool SVONVolume::IsReadyForNavigation()
{
	return isReadyFOrNavigation;
}

void SVONVolume::SetupVolume()
{
	bool succeed = false;
	if (GetVolumBoudingBox != nullptr)
	{
		succeed = GetVolumBoudingBox(origin, origin);
	}

	if (!succeed)
	{
		origin = FloatVector();
		extent = FloatVector(100, 100, 100);
	}
}

bool SVONVolume::FirstPassRasterize()
{
	if (OverlapBoxBlockingTest == nullptr)
	{
		return false;
	}

	// Add the first layer of blocking
	blockedIndices.clear();

	int32_t numNodes = GetNodesInLayer(1);
	for (int32_t i = 0; i < numNodes; ++i)
	{
		FloatVector position;
		GetNodePosition(1, i, position);

		float boxRadius = GetVoxelSize(1) * 0.5f;
		if (OverlapBoxBlockingTest(position, boxRadius, collisionLayers))
		{
			blockedIndices[0].insert(i);
		}
	}

	int layerIndex = 0;
	while (blockedIndices[layerIndex].size() > 1)
	{
		// Add a new layer to structure
		blockedIndices.push_back(std::set<mortoncode_t>());

		// Add any parent morton codes to the new layer
		for (const mortoncode_t code : blockedIndices[layerIndex])
		{
			blockedIndices[layerIndex + 1].insert(code >> 3);
		}
		++layerIndex;
	}

	return true;
}

void SVONVolume::RasterizeLayer(layerindex_t aLayer)
{
	nodeindex_t leafIndex = 0;
	// Layer 0 Leaf nodes are special
	if (aLayer == 0)
	{
		// Run through all our coordinates
		int32_t numNodes = GetNodesInLayer(aLayer);
		for (int32_t i = 0; i < numNodes; ++i)
		{
			int index = i;

			// If we know this node needs to be added, from the low res first pass
			if (blockedIndices[0].find(i >> 3) != blockedIndices[0].end())
			{
				// Add a node
				std::vector<SVONNode>& nodes = GetLayer(aLayer);
				nodes.push_back(SVONNode());
				index = nodes.size() - 1;
				SVONNode& node = nodes[index];

				// Set code and position
				node.code = i;

				FloatVector nodePos;
				GetNodePosition(aLayer, node.code, nodePos);

				//TODO: Add debug stuff

				// Now check if we have any blockings, and search leaf nodes
				FloatVector position;
				GetNodePosition(0, i, position);// ?? just ust nodePos ??

				if (IsBlocked(position, GetVoxelSize(0) * 0.5f))
				{
					// Rasterize leaf nodes
					FloatVector leafOrigin = nodePos - FloatVector(GetVoxelSize(aLayer) * 0.5f);
					RasterizeLeafNode(leafOrigin, leafIndex);
					node.firstChild.SetLayerIndex(0);
					node.firstChild.SetNodeIndex(leafIndex);
					node.firstChild.SetSubnodeIndex(0);
					++leafIndex;
				}
				else
				{
					data.leafNodes.push_back(SVONLeafNode());
					++leafIndex;
					node.firstChild.SetInvalid();
				}
			}
		}
	}
	// Deal with the other layers
	else if (GetLayer(aLayer - 1).size() > 1)
	{
		int nodeCounter = 0;
		int32_t numNodes = GetNodesInLayer(aLayer);
		for (int32_t i = 0; i < numNodes; ++i)
		{
			// Do we have any blocking children, or siblings?
			// Remember we must have 8 children per parent
			if (IsAnyMemberBlocked(aLayer, i))
			{
				// Add a node
				std::vector<SVONNode>& nodes = GetLayer(aLayer);
				nodes.push_back(SVONNode());
				int32_t index = nodes.size() - 1;
				SVONNode& node = nodes[index];

				// Set details
				node.code = i;
				nodeindex_t childIndex = 0;
				if (GetIndexForCode(aLayer - 1, node.code << 3, childIndex))
				{
					// Set parent->child links
					node.firstChild.SetLayerIndex(aLayer - 1);
					node.firstChild.SetNodeIndex(childIndex);

					// Set child->parent links, this can probably be done smarter, 
					// as we're duplicating work here
					for (int iter = 0; iter < 8; ++iter)
					{
						SVONLink& parent = GetLayer(node.firstChild.GetLayerIndex())[node.firstChild.GetNodeIndex() + iter].parent;
						parent.SetLayerIndex(aLayer);
						parent.SetNodeIndex(index);
					}

					// TODO: Debug all the things
				}
				else
				{
					node.firstChild.SetInvalid();
				}

				// TODO: Debug showMortonCodes || showVoxels
			}
		}
	}
}

int32_t SVONVolume::GetNodesInLayer(layerindex_t aLayer)
{
	return static_cast<int32_t>( powf(powf(2, static_cast<float>(voxelPower - aLayer)), 3) );
}

int32_t SVONVolume::GetNodesPerSide(layerindex_t aLayer)
{
	return static_cast<int32_t>( powf(2, static_cast<float>(voxelPower - aLayer)) );
}

bool SVONVolume::GetIndexForCode(layerindex_t aLayer, mortoncode_t aCode, nodeindex_t& oIndex) const
{
	const std::vector<SVONNode>& layer = GetLayer(aLayer);

	for (size_t i = 0; i < layer.size(); ++i)
	{
		if (layer[i].code == aCode)
		{
			oIndex = i;
			return true;
		}
	}

	return false;
}

void SVONVolume::BuildNeighbourLinks(layerindex_t aLayer)
{
	std::vector<SVONNode>& layer = GetLayer(aLayer);
	layerindex_t searchLayer = aLayer;

	// For each node
	for (size_t i = 0; i < layer.size(); ++i)
	{
		SVONNode& node = layer[i];
		// Get our world coordinate
		uint_fast32_t x, y, z;
		morton3D_64_decode(node.code, x, y, z);
		nodeindex_t backtrackIndex = -1;
		nodeindex_t index = i;
		FloatVector nodePos;
		GetNodePosition(aLayer, node.code, nodePos);

		// For each direction
		for (int d = 0; d < 6; ++d)
		{
			SVONLink& linkToUpdate = node.neighbours[d];

			backtrackIndex = index;

			while (!FindLinkInDirection(searchLayer, index, d, linkToUpdate, nodePos)
				&& aLayer < data.layers.size() - 2)
			{
				SVONLink& parent = GetLayer(searchLayer)[index].parent;
				if (parent.IsValid())
				{
					index = parent.nodeIndex;
					searchLayer = parent.layerIndex;
				}
				else
				{
					searchLayer++;
					GetIndexForCode(searchLayer, node.code >> 3, index);
				}
			}

			index = backtrackIndex;
			searchLayer = aLayer;
		}
	}
}

bool SVONVolume::FindLinkInDirection(layerindex_t aLayer, const nodeindex_t aNodeIndex, 
	uint8_t aDir, SVONLink& oLinkToUpdate, FloatVector& aStartPosForDebug)
{
	int32_t maxCoord = GetNodesPerSide(aLayer);
	SVONNode& node = GetLayer(aLayer)[aNodeIndex];
	std::vector<SVONNode>& layer = GetLayer(aLayer);

	// Get our world coordinate
	uint_fast32_t x = 0, y = 0, z = 0;
	morton3D_64_decode(node.code, x, y, z);
	int32_t sX = x, sY = y, sZ = z;
	// Add the direction
	sX += SVONStatics::dirs[aDir].X;
	sY += SVONStatics::dirs[aDir].Y;
	sZ += SVONStatics::dirs[aDir].Z;

	// If the coords are out of bounds, the link is invalid
	if (sX < 0 || sX >= maxCoord || sY < 0 || sY >= maxCoord || sZ < 0 || sZ >= maxCoord)
	{
		oLinkToUpdate.SetInvalid();
		// TODO: Debug stuff
		return true;
	}
	x = sX, y = sY, z = sZ;
	// Get the morton code for the direction
	mortoncode_t thisCode = morton3D_64_encode(x, y, z);
	bool isHigher = thisCode > node.code;
	int32_t nodeDelta = (isHigher ? 1 : -1);

	while ((aNodeIndex + nodeDelta) < static_cast<int32_t>(layer.size())
		&& aNodeIndex + nodeDelta >= 0)
	{
		// This is the node we're looking for
		if (layer[aNodeIndex + nodeDelta].code == thisCode)
		{
			const SVONNode& thisNode = layer[aNodeIndex + nodeDelta];
			// This is a leaf node
			if (aLayer == 0 && thisNode.HasChildren())
			{
				// Set invalid link if the leaf node is completely blocked, no point linking to it
				if (GetLeafNode(thisNode.firstChild.GetNodeIndex()).IsCompletelyBlocked())
				{
					oLinkToUpdate.SetInvalid();
					return true;
				}
			}

			// Otherwise, use this link
			oLinkToUpdate.layerIndex = aLayer;
			assert(aNodeIndex + nodeDelta < static_cast<int32_t>(layer.size()));
			oLinkToUpdate.nodeIndex = aNodeIndex + nodeDelta;
			// TODO: debug stuff
			return true;
		}
		// If we've passed the code we're looking for, it's not on this layer
		else if ((isHigher && layer[aNodeIndex + nodeDelta].code > thisCode)
			|| (!isHigher && layer[aNodeIndex + nodeDelta].code < thisCode))
		{
			return false;
		}

		nodeDelta += (isHigher ? 1 : -1);
	}

	// I'm not entirely sure if it's valid to reach the end? Hmmm...
	return false;
}

// Gets the position of a given link. Returns true if the link is open, false if blocked
bool SVONVolume::GetLinkPosition(const SVONLink& aLink, FloatVector& oPosition) const
{
	const SVONNode& node = GetLayer(aLink.GetLayerIndex())[aLink.GetNodeIndex()];

	GetNodePosition(aLink.GetLayerIndex(), node.code, oPosition);
	// If this is layer 0, and there are valid children
	if (aLink.GetLayerIndex() == 0 && node.firstChild.IsValid())
	{
		float voxelSize = GetVoxelSize(0);
		uint_fast32_t x, y, z;
		morton3D_64_decode(aLink.GetSubnodeIndex(), x, y, z);
		oPosition += FloatVector(x * voxelSize * 0.25f,
			y * voxelSize * 0.25f,
			z * voxelSize * 0.25f)
			- FloatVector(voxelSize * 0.375f);//= -voxelSize * 0.5f + voxelSize * 0.125f
		const SVONLeafNode& leafNode = GetLeafNode(node.firstChild.nodeIndex);
		bool isBlocked = leafNode.GetNode(aLink.GetSubnodeIndex());
		return !isBlocked;
	}

	return true;
}

bool SVONVolume::GetNodePosition(layerindex_t aLayer, mortoncode_t aCode,
	FloatVector& oPosition) const
{
	float voxelSize = GetVoxelSize(aLayer);
	uint_fast32_t x, y, z;
	morton3D_64_decode(aCode, x, y, z);
	oPosition = origin - extent
		+ FloatVector(x * voxelSize, y * voxelSize, z * voxelSize)
		+ FloatVector(voxelSize * 0.5f);
	return true;
}

const SVONNode& SVONVolume::GetNode(const SVONLink& aLink) const
{
	if (aLink.GetLayerIndex() < 14)
	{
		return GetLayer(aLink.GetLayerIndex())[aLink.GetNodeIndex()];
	}
	else
	{
		return GetLayer(numLayers - 1)[0];
	}
}

const SVONLeafNode& SVONVolume::GetLeafNode(nodeindex_t aIndex) const
{
	return data.leafNodes[aIndex];
}

void SVONVolume::GetLeafNeighbours(const SVONLink& aLink,
	std::vector<SVONLink>& oNeighbours) const
{
	mortoncode_t leafIndex = aLink.GetSubnodeIndex();
	const SVONNode& node = GetNode(aLink);
	const SVONLeafNode& leaf = GetLeafNode(node.firstChild.GetNodeIndex());

	// Get our starting co-ordinates
	uint_fast32_t x = 0, y = 0, z = 0;
	morton3D_64_decode(leafIndex, x, y, z);

	for (int i = 0; i < 6; ++i)
	{
		// Need to switch to singed ints
		int32_t sX = x + SVONStatics::dirs[i].X;
		int32_t sY = x + SVONStatics::dirs[i].Y;
		int32_t sZ = x + SVONStatics::dirs[i].Z;

		// If the neighbour is in bounds of this leaf node
		if (sX >= 0 && sX < 4 && sY >= 0 && sY < 4 && sZ >= 0 && sZ < 4)
		{
			mortoncode_t thisIndex = morton3D_64_encode(sX, sY, sZ);
			// If this node is blocked, then no link in this direction, continue
			if (leaf.GetNode(thisIndex))
			{
				continue;
			}
			else // Otherwise, this is a valid link, add it
			{
				oNeighbours.push_back(SVONLink(0, aLink.GetNodeIndex(), static_cast<uint8_t>(thisIndex)));
				continue;
			}
		}
		else // the neighbours is out of bounds, we need to find our neighbour
		{
			const SVONLink& neighbourLink = node.neighbours[i];
			const SVONNode& neighbourNode = GetNode(neighbourLink);

			// If the neighbour layer 0 has no leaf nodes, just return it
			if (!neighbourNode.firstChild.IsValid())
			{
				oNeighbours.push_back(neighbourLink);
				continue;
			}

			const SVONLeafNode& leafNode = GetLeafNode(neighbourNode.firstChild.GetNodeIndex());
			if (leafNode.IsCompletelyBlocked())
			{
				// The leaf node is completely blocked, we don't return it
				continue;
			}
			else // Otherwise, we need to find the correct subnode
			{
				if (sX < 0)
					sX = 3;
				else if (sX > 3)
					sX = 0;
				else if (sY < 0)
					sY = 3;
				else if (sY > 3)
					sY = 0;
				else if (sZ < 0)
					sZ = 3;
				else if (sZ > 3)
					sZ = 0;

				mortoncode_t subNodeCode = morton3D_64_encode(sX, sY, sZ);

				// Only return the neighbour if it isn't blocked
				if (!leafNode.GetNode(subNodeCode))
				{
					oNeighbours.push_back(SVONLink(0,
						neighbourNode.firstChild.GetNodeIndex(),
						static_cast<uint8_t>(subNodeCode)));
				}
			}
		}
	}
}

void SVONVolume::GetNeighbours(const SVONLink& aLink, std::vector<SVONLink>& oNeighbours) const
{
	const SVONNode& node = GetNode(aLink);

	for (int i = 0; i < 6; ++i)
	{
		const SVONLink& neighbourLink = node.neighbours[i];

		if (!neighbourLink.IsValid())
		{
			continue;
		}

		const SVONNode& neighbour = GetNode(neighbourLink);

		// If the neighbour has no children, it's empty, we just use it
		if (!neighbour.HasChildren())
		{
			oNeighbours.push_back(neighbourLink);
			continue;
		}

		// If the node has children, we need to look down the tree
		// to see which children we want to add to the neighbour set

		// Start working set, and put hte link into it
		std::queue<SVONLink> workingSet;
		workingSet.push(neighbourLink);

		while (workingSet.size() > 0)
		{
			// Pop off the top of the working set
			SVONLink thisLink = workingSet.front();
			workingSet.pop();
			const SVONNode& thisNode = GetNode(thisLink);

			// If the node has no children, it's clear, so add to neighbours and continue
			if (!thisNode.HasChildren())
			{
				oNeighbours.push_back(neighbourLink);
				continue;
			}

			// We know it has children
			if (thisLink.GetLayerIndex() > 0)
			{
				// If it's abouve layer 0, we will need to potentially add 4 children using our offests
				for (const nodeindex_t& childIndex : SVONStatics::dirChildOffsets[i])
				{
					// Each of the childnodes
					SVONLink childLink = thisNode.firstChild;
					childLink.nodeIndex += childIndex;
					const SVONNode& childNode = GetNode(childLink);

					if (childNode.HasChildren())
					{
						// If it has children, add them tho the working set to keep going donw
						workingSet.push(childLink);
					}
					else
					{
						// Or just add to the outgong links
						oNeighbours.push_back(childLink);
					}
				}
			}
			else
			{
				// If this is a leaf layer, then we need to add whicherver 
				// of the 16 facing leaf nodes aren't blocked
				for (const nodeindex_t& leafIndex : SVONStatics::dirLeafChildOffsets[i])
				{
					// Each of the childnodes
					SVONLink link = neighbour.firstChild;
					const SVONLeafNode& leafNode = GetLeafNode(link.nodeIndex);
					link.subnodeIndex = leafIndex;

					if (!leafNode.GetNode(leafIndex))
					{
						oNeighbours.push_back(link);
					}
				}
			}
		}
	}
}

void SVONVolume::ClearData()
{
	data.Reset();
	numLayers = 0;
	numBytes = 0;
}

void SVONVolume::RasterizeLeafNode(FloatVector& aOrigin, nodeindex_t aLeafIndex)
{
	float leafVoxelSize = GetVoxelSize(0) * 0.25f;
	for (int i = 0; i < 64; ++i)
	{
		uint_fast32_t x, y, z;
		morton3D_64_decode(i, x, y, z);
		FloatVector position = aOrigin
			+ FloatVector(x * leafVoxelSize, y * leafVoxelSize, z * leafVoxelSize)
			+ FloatVector(leafVoxelSize * 0.5f);

		if (aLeafIndex > static_cast<nodeindex_t>(data.leafNodes.size()))
		{
			data.leafNodes.push_back(SVONLeafNode());
		}

		if (IsBlocked(position, leafVoxelSize * 0.5f))
		{
			data.leafNodes[aLeafIndex].SetNode(i);

			// TODO: Debug stuff
		}
	}
}

// Check for blocking...using this cached set for each layer for now for fast lookups
bool SVONVolume::IsAnyMemberBlocked(layerindex_t aLayer, mortoncode_t aCode)
{
	if (aLayer == blockedIndices.size())
	{
		return true;
	}

	// The parent of this code is blocked
	mortoncode_t parentCode = aCode >> 3;
	if (blockedIndices[aLayer].find(parentCode) != blockedIndices[aLayer].end())
	{
		return true;
	}

	return false;
}

bool SVONVolume::IsBlocked(const FloatVector& aPositon, const float aSize) const
{
	if (OverlapBoxBlockingTest == nullptr)
	{
		return false;
	}
	
	return OverlapBoxBlockingTest(aPositon, aSize, collisionLayers);
}
