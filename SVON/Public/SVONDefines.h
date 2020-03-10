#pragma once

#include <stdint.h>
#include "SVONAPI.h"
#include "SVONMath.h"
#include "SVONColor.h"

typedef uint8_t layerindex_t;
typedef int32_t nodeindex_t;
typedef uint8_t subnodeindex_t;
typedef uint_fast64_t mortoncode_t;
typedef uint_fast32_t posint_t;

#define LEAF_LAYER_INDEX 14;

namespace SVON
{
	enum class dir : uint8_t
	{
		pX, nX, pY, nY, pZ, nZ
	};

	class SVONStatics
	{
	public:
		static const IntVector dirs[];
		static const nodeindex_t dirChildOffsets[6][4];
		static const nodeindex_t dirLeafChildOffsets[6][16];
		static const Color layerColors[];
		static const Color linkColors[];
	};

	namespace PathfindingRequestResultType
	{
		enum Type
		{
			Failed, // Something went wrong
			ReadyToPath, // Pre-reqs satisfied
			AlreadyAtGoal, // No need to move
			Deferred, // Passed request to another thread, need to wait
			Success // it worked!
		};
	}
}
