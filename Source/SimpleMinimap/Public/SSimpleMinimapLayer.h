// One batch of minimap markers: one quad + an instance buffer (one packed FVector4f per marker,
// SimpleMinimapPacking.h), drawn with FSlateDrawElement::MakeCustomVerts = one Slate batch, one draw.
// Slate instances only through a material (the non-material path has check(!bUseInstancing)), so the
// material places, rotates and textures every instance -- Shaders/Private/SimpleMinimapMarker.ush.
// All markers of a layer share its material, so its atlas: one atlas = one layer.

#pragma once

#include "CoreMinimal.h"
#include "Rendering/RenderingCommon.h"
#include "SlateMaterialBrush.h"
#include "Widgets/SLeafWidget.h"

class UMaterialInterface;

class SIMPLEMINIMAP_API SSimpleMinimapLayer : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SSimpleMinimapLayer)
		: _Material(nullptr)
		{}
		/** UI-domain material reading the instance data (an instance of M_SimpleMinimapMarker). */
		SLATE_ARGUMENT(UMaterialInterface*, Material)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetMaterial(UMaterialInterface* InMaterial);
	/** One packed float4 per marker. */
	void SetInstances(FSlateInstanceBufferData&& Data);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;

private:
	TOptional<FSlateMaterialBrush> Brush;
	mutable FSlateResourceHandle ResourceHandle;
	TSharedPtr<ISlateUpdatableInstanceBuffer> InstanceBuffer;
	int32 NumInstances = 0;
};
