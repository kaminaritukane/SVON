#pragma once

#include <vector>
#include <set>
#include <functional>
#include "SVONDefines.h"
#include "SVONData.h"

namespace SVON
{
	enum class SVOGenerationStrategy : uint8_t
	{
		UseBaked,
		GenerateOnBeginPlay
	};

	// callback funtions
	typedef std::function<bool(FloatVector& origin, 
		FloatVector& extent)> GetVolumBoudingBoxFunc;

	typedef std::function<bool(const FloatVector& pos, 
		float boxRadius,
		uint32_t layers)> OverlapBoxBlockingTestFunc;

	class SVONVolume
	{
	public:
		SVONVolume(int32_t aVoxelPower, 
			GetVolumBoudingBoxFunc getVolumBoudingBoxFunc,
			OverlapBoxBlockingTestFunc boxOverlapCheckFunc)
			: voxelPower(aVoxelPower)
			, GetVolumBoudingBox(getVolumBoudingBoxFunc)
			, OverlapBoxBlockingTest(boxOverlapCheckFunc)
		{

		}

		//float debugDistance = 5000.0f;
		//bool showVoxels = false;
		//bool showLeafVoxels = false;
		//bool showMortonCodes = false;
		//bool showNeighbourLinks = false;
		int32_t voxelPower = 2;

		uint32_t collisionLayers = -1;
		//float clearance = 0.f;

		SVOGenerationStrategy generationStrategy = SVOGenerationStrategy::UseBaked;
		uint8_t numLayers = 0;
		int numBytes = 0;

		void SetGetVolumBoudingBoxFunc(GetVolumBoudingBoxFunc cb)
		{
			GetVolumBoudingBox = cb;
		}
		void SetOverlapBoxBlockingTestFunc(OverlapBoxBlockingTestFunc cb)
		{
			OverlapBoxBlockingTest = cb;
		}

		bool Generate();

		const FloatVector& GetOrigin() const { return origin; }
		const FloatVector& GetExtent() const { return extent; }
		const uint8_t GetNumLayers() const { return numLayers; }
		const std::vector<SVONNode>& GetLayer(layerindex_t aLayer) const;
		float GetVoxelSize(layerindex_t aLayer) const;

		bool IsReadyForNavigation();

		// Gets the position of a given link. Returns true if the link is open, false if blocked
		bool GetLinkPosition(const SVONLink& aLink, FloatVector& oPosition) const;
		bool GetNodePosition(layerindex_t aLayer, mortoncode_t aCode,
			FloatVector& oPosition) const;
		const SVONNode& GetNode(const SVONLink& aLink) const;
		const SVONLeafNode& GetLeafNode(nodeindex_t aIndex) const;

		void GetLeafNeighbours(const SVONLink& aLink, 
			std::vector<SVONLink>& oNeighbours) const;
		void GetNeighbours(const SVONLink& aLink, std::vector<SVONLink>& oNeighbours) const;

		void ClearData();

	private:
		bool isReadyFOrNavigation = false;

		FloatVector origin;
		FloatVector extent;

		FloatVector debugPosition;

		SVONData data;

		// First pass rasterize results
		std::vector<std::set<mortoncode_t>> blockedIndices;

		// Callback functions
		GetVolumBoudingBoxFunc GetVolumBoudingBox = nullptr;
		OverlapBoxBlockingTestFunc OverlapBoxBlockingTest = nullptr;

		std::vector<SVONNode>& GetLayer(layerindex_t aLayer);

		// Get Volume's origin and extent, using gameobject's bounding box
		void SetupVolume();

		bool FirstPassRasterize();
		void RasterizeLayer(layerindex_t aLayer);

		int32_t GetNodesInLayer(layerindex_t aLayer);
		int32_t GetNodesPerSide(layerindex_t aLayer);

		bool GetIndexForCode(layerindex_t aLayer, mortoncode_t aCode, nodeindex_t& oIndex) const;

		void BuildNeighbourLinks(layerindex_t aLayer);
		bool FindLinkInDirection(layerindex_t aLayer, const nodeindex_t aNodeIndex,
			uint8_t aDir, SVONLink& oLinkToUpdate, FloatVector& aStartPosForDebug);
		void RasterizeLeafNode(FloatVector& aOrigin, nodeindex_t aLeafIndex);

		// Check for blocking...using this cached set for each layer for now for fast lookups
		bool IsAnyMemberBlocked(layerindex_t aLayer, mortoncode_t aCode);
		bool IsBlocked(const FloatVector& aPositon, const float aSize) const;
	};
}