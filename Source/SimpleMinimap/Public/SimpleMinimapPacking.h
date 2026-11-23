// SimpleMinimap instance packing: one marker = one FVector4f (Slate gives exactly one float4 per instance).
// Mirror of Shaders/Private/SimpleMinimapMarker.ush -- change both together.
//
//   X = PosX  * 4096 + PosY        12 + 12 bits, radar position, -1..1 mapped to 0..4095
//   Y = Angle * 256  + ColorIndex  16 +  8 bits, angle 0..65535 = 0..360 deg clockwise; palette index
//   Z = U     * 4096 + V           12 + 12 bits, sprite rect origin in atlas pixels
//   W = Width * 4096 + Height      12 + 12 bits, sprite rect size in atlas pixels
//
// Every component is a whole number below 2^24, so a float stores it exactly and no bits are ever
// reinterpreted (no NaN / denormal payloads). The vertex shader decodes it with float maths in fp32;
// the pixel shader only gets plain small values -- safe for mobile half precision.
// Limits that follow: position step 2/4095 radar units, angle step ~0.0055 deg, 256 palette
// entries (the material uses 8), atlas and sprites up to 4095 px.

#pragma once

#include "CoreMinimal.h"

namespace SimpleMinimap
{
	inline float PackPair(uint32 Hi, uint32 Lo, uint32 LoBits)
	{
		return static_cast<float>((Hi << LoBits) | Lo);
	}

	/** Position in radar units (centre 0, edge 1, +Y down); values outside -1..1 are clamped. */
	inline float PackPosition(FVector2f Position)
	{
		auto Quantize = [](float V) { return static_cast<uint32>(FMath::RoundToInt(FMath::Clamp(V * 0.5f + 0.5f, 0.f, 1.f) * 4095.f)); };
		return PackPair(Quantize(Position.X), Quantize(Position.Y), 12);
	}

	/** Angle in degrees, clockwise on screen; palette index 0..255. */
	inline float PackAngleColor(float AngleDegrees, int32 ColorIndex)
	{
		const float Turns = FMath::Fmod(AngleDegrees / 360.f, 1.f);
		const uint32 Angle = static_cast<uint32>(FMath::RoundToInt((Turns < 0.f ? Turns + 1.f : Turns) * 65536.f)) & 0xFFFFu;
		return PackPair(Angle, static_cast<uint32>(FMath::Clamp(ColorIndex, 0, 255)), 8);
	}

	/** Rect in atlas pixels; each value 0..4095. */
	inline FVector2f PackRect(FIntRect RectPx)
	{
		auto C = [](int32 V) { return static_cast<uint32>(FMath::Clamp(V, 0, 4095)); };
		return FVector2f(PackPair(C(RectPx.Min.X), C(RectPx.Min.Y), 12), PackPair(C(RectPx.Width()), C(RectPx.Height()), 12));
	}

	inline FVector4f PackMarker(FVector2f Position, float AngleDegrees, int32 ColorIndex, FIntRect RectPx)
	{
		const FVector2f Rect = PackRect(RectPx);
		return FVector4f(PackPosition(Position), PackAngleColor(AngleDegrees, ColorIndex), Rect.X, Rect.Y);
	}
}
