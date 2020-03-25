#pragma once

#include "SVONDefines.h"
#include "SVONLink.h"

namespace SVON
{
	struct SVONNode
	{
		mortoncode_t code = 0;

		SVONLink parent;
		SVONLink firstChild;// firstChild.Invalid == true, means no geometry in this node

		SVONLink neighbours[6];// neighbour[x].Invalid == true, means no neighbour(blocked or out of range

		SVONNode()
			: parent(SVONLink::GetInvalidLink())
			, firstChild(SVONLink::GetInvalidLink())
		{}

		bool HasChildren() const { return firstChild.IsValid(); }
	};
}