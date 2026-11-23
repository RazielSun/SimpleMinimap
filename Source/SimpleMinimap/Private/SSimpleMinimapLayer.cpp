#include "SSimpleMinimapLayer.h"

#include "Framework/Application/SlateApplication.h"
#include "Interfaces/ISlateRHIRendererModule.h"
#include "Materials/MaterialInterface.h"
#include "Modules/ModuleManager.h"
#include "Rendering/DrawElements.h"
#include "Rendering/SlateRenderer.h"

void SSimpleMinimapLayer::Construct(const FArguments& InArgs)
{
	SetMaterial(InArgs._Material);
}

void SSimpleMinimapLayer::SetMaterial(UMaterialInterface* InMaterial)
{
	Brush.Reset();
	ResourceHandle = FSlateResourceHandle();
	if (InMaterial)
	{
		Brush.Emplace(*InMaterial, FVector2D(1.0, 1.0));
	}
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SSimpleMinimapLayer::SetInstances(FSlateInstanceBufferData&& Data)
{
	NumInstances = Data.Num();
	if (NumInstances > 0)
	{
		if (!InstanceBuffer.IsValid())
		{
			InstanceBuffer = FModuleManager::GetModuleChecked<ISlateRHIRendererModule>(TEXT("SlateRHIRenderer")).CreateInstanceBuffer(NumInstances);
		}
		InstanceBuffer->Update(Data);
	}
	Invalidate(EInvalidateWidgetReason::Paint);
}

int32 SSimpleMinimapLayer::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (NumInstances == 0 || !InstanceBuffer.IsValid() || !Brush.IsSet())
	{
		return LayerId;
	}
	if (!ResourceHandle.IsValid())
	{
		ResourceHandle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(*Brush);
		if (!ResourceHandle.IsValid())
		{
			return LayerId;
		}
	}

	// Radar centre and radius in window pixels. This widget's render transform is not applied:
	// the material places the quads itself from these two values.
	const FVector2f LocalSize = FVector2f(AllottedGeometry.GetLocalSize());
	const FVector2f Center = FVector2f(AllottedGeometry.LocalToAbsolute(FVector2D(LocalSize * 0.5f)));
	const float Radius = 0.5f * FMath::Min(LocalSize.X, LocalSize.Y) * AllottedGeometry.Scale;

	static const FVector2f Corners[4] = { { -0.5f, -0.5f }, { 0.5f, -0.5f }, { 0.5f, 0.5f }, { -0.5f, 0.5f } };
	TArray<FSlateVertex> Verts;
	Verts.SetNumUninitialized(4);
	for (int32 Index = 0; Index < 4; ++Index)
	{
		FSlateVertex& Vertex = Verts[Index];
		Vertex = FSlateVertex();
		Vertex.TexCoords[0] = Corners[Index].X; // TexCoord0: corner
		Vertex.TexCoords[1] = Corners[Index].Y;
		Vertex.TexCoords[2] = Center.X;         // TexCoord1: radar centre
		Vertex.TexCoords[3] = Center.Y;
		Vertex.MaterialTexCoords = FVector2f(Radius, 0.f); // TexCoord4: radar radius
		Vertex.Position = Center + Corners[Index] * 2.f * Radius; // replaced by the material's Screen Position
		Vertex.Color = FColor::White;
		Vertex.SecondaryColor = FColor::White;
	}
	static const TArray<SlateIndex> Indices = { 0, 1, 2, 0, 2, 3 };

	FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, ResourceHandle, Verts, Indices,
		InstanceBuffer.Get(), 0, NumInstances);
	return LayerId;
}

FVector2D SSimpleMinimapLayer::ComputeDesiredSize(float) const
{
	return FVector2D(1.0, 1.0);
}
