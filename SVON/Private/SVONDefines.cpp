#include "SVONDefines.h"

using namespace SVON;

// In UE4, Left Hand Rule is used,
// The X axis is to the right directiong
// The Y axis is towards me, 
// The Z axis is upwards
const IntVector SVONStatics::dirs[] = {
	IntVector(1,0,0),//right
	IntVector(-1,0,0),//left
	IntVector(0,1,0),//towards me
	IntVector(0,-1,0),//backwards from me
	IntVector(0,0,1),//up
	IntVector(0,0,-1)//donw
};

const nodeindex_t SVONStatics::dirChildOffsets[6][4] = {
	{ 0,4,2,6 },
	{ 1,3,5,7 },
	{ 0,1,4,5 },
	{ 2,3,6,7 },
	{ 0,1,2,3 },
	{ 4,5,6,7 }
};

const nodeindex_t SVONStatics::dirLeafChildOffsets[6][16] = {
	{ 0 ,2 ,16,18 ,4 ,6 ,20,22 ,32,34,48,50 ,36,38,52,54 },
	{ 9 ,11,25,27 ,13,15,29,31 ,41,43,57,59 ,45,47,61,63 },
	{ 0 ,1 ,8 ,9  ,4 ,5 ,12,13 ,32,33,40,41 ,36,37,44,45 },
	{ 18,19,26,27 ,22,23,30,31 ,50,51,58,59 ,54,55,62,63 },
	{ 0 ,1 ,8 ,9  ,2 ,3 ,10,11 ,16,17,24,25 ,18,19,26,27 },
	{ 36,37,44,45 ,38,39,46,47 ,52,53,60,61 ,54,55,62,63 }
};

const Color SVONStatics::layerColors[] = {
	Color::Orange,
	Color::Yellow, 
	Color::White, 
	Color::Blue, 
	Color::Turquoise, 
	Color::Cyan, 
	Color::Emerald, 
	Color::Orange
};

const Color SVONStatics::linkColors[] = {
	Color(0xFF000000),
	Color(0xFF444444),
	Color(0xFF888888),
	Color(0xFFBBBBBB),
	Color(0xFFFFFFFF),
	Color(0xFF999999),
	Color(0xFF777777),
	Color(0xFF555555)
};