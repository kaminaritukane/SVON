#pragma once

#include "SVONDefines.h"
#include "SVONLink.h"

namespace SVON
{
	struct SVONNode
	{
		mortoncode_t code = 0;

		SVONLink parent;
		SVONLink firstChild;

		SVONLink neighbours[6];

		SVONNode()
			: parent(SVONLink::GetInvalidLink())
			, firstChild(SVONLink::GetInvalidLink())
		{}

		bool HasChildren() const { return firstChild.IsValid(); }
	};
}