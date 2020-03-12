#pragma once

#include <list>
#include <map>
#include "SVONLink.h"
#include "SVONNavigationPath.h"

namespace SVON
{
	struct FloatVector;
	struct SVONNavigationPath;
	class SVONVolume;

	struct SVONPathFinderSettings
	{
		bool useUnitCost;
		float unitCost;
		float estimateWeight;
		float nodeSizeCompensation;
		SVONPathCostType pathCostType;

		struct SVONPathFinderSettings()
			: useUnitCost(false)
			, unitCost(1.0f)
			, estimateWeight(1.0f)
			, nodeSizeCompensation(1.0f)
			, pathCostType(SVONPathCostType::EUCLIDEAN)
		{}
	};

	class SVONPathFinder
	{
		typedef std::list<SVONLink> LinkList;
		typedef std::map<SVONLink, SVONLink> LinksMap;
		typedef std::map<SVONLink, float> LinkScoreMap;

	public:
		SVONPathFinder(const SVONVolume& aVolume, const SVONPathFinderSettings& aSettings)
			: volume(aVolume)
			, settings(aSettings)
		{};

		~SVONPathFinder() {};

		// Performs an A* search from start to target navlink
		bool FindPath(const SVONLink& aStart,
			const SVONLink& aTarget,
			const FloatVector& aStartPos,
			const FloatVector& aTargetPos,
			SVONNavigationPath& oPath);

	private:
		LinkList openList;
		LinkList closedList;

		LinksMap cameFrom;

		LinkScoreMap gScore;
		LinkScoreMap fScore;

		SVONLink start;
		SVONLink current;
		SVONLink goal;

		const SVONVolume& volume;

		SVONPathFinderSettings settings;

		// A* heursistic calculation
		float HeuristicScore(const SVONLink& aStart, const SVONLink& aTarget);

		// Distance between two links
		float GetCost(const SVONLink& aStart, const SVONLink& aTarget);

		void ProcessLink(const SVONLink& aNeighbour);

		// Constructs the path by navigationg back through our CameFrom map
		void BuildPath(LinksMap& aCameFrom, SVONLink aCurrent,
			const FloatVector& aStartPos, const FloatVector& aTargetPos,
			SVONNavigationPath& oPath);
	};
}