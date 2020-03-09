#pragma once
#include <stdint.h>

namespace SVON
{
	struct Color
	{
	public:
		union { struct { uint8_t B, G, R, A; }; uint32_t AlignmentDummy = 0; };

		uint32_t& DWColor(void) { return *((uint32_t*)this); }
		const uint32_t& DWColor(void) const { return *((uint32_t*)this); }

		__forceinline Color() {}

		__forceinline explicit Color(uint32_t InColor)
		{
			DWColor() = InColor;
		}

		constexpr __forceinline Color(uint8_t InR, uint8_t InG, uint8_t InB, uint8_t InA = 255)
			: B(InB), G(InG), R(InR), A(InA)
		{
		}

		static const Color White;
		static const Color Black;
		static const Color Transparent;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;
		static const Color Cyan;
		static const Color Magenta;
		static const Color Orange;
		static const Color Purple;
		static const Color Turquoise;
		static const Color Silver;
		static const Color Emerald;
	};

	const Color Color::White(255, 255, 255);
	const Color Color::Black(0, 0, 0);
	const Color Color::Transparent(0, 0, 0, 0);
	const Color Color::Red(255, 0, 0);
	const Color Color::Green(0, 255, 0);
	const Color Color::Blue(0, 0, 255);
	const Color Color::Yellow(255, 255, 0);
	const Color Color::Cyan(0, 255, 255);
	const Color Color::Magenta(255, 0, 255);
	const Color Color::Orange(243, 156, 18);
	const Color Color::Purple(169, 7, 228);
	const Color Color::Turquoise(26, 188, 156);
	const Color Color::Silver(189, 195, 199);
	const Color Color::Emerald(46, 204, 113);
}