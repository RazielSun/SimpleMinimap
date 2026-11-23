#include "SimpleMinimapWidget.h"

#include "Engine/Texture.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SimpleMinimapMarkerComponent.h"
#include "SimpleMinimapModule.h"
#include "SimpleMinimapPacking.h"
#include "SimpleMinimapSubsystem.h"
#include "SSimpleMinimapLayer.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SOverlay.h"

#define LOCTEXT_NAMESPACE "SimpleMinimap"

namespace SimpleMinimap
{
	constexpr int32 MaterialPaletteSize = 8;
}

USimpleMinimapWidget::USimpleMinimapWidget()
{
	Palette = {
		FLinearColor::White,
		FLinearColor(0.95f, 0.25f, 0.2f),
		FLinearColor(0.3f, 0.85f, 0.35f),
		FLinearColor(0.25f, 0.6f, 1.0f),
		FLinearColor(1.0f, 0.8f, 0.15f),
		FLinearColor(0.75f, 0.35f, 1.0f),
		FLinearColor(1.0f, 0.5f, 0.15f),
		FLinearColor(0.2f, 0.9f, 0.9f),
	};
}

TSharedRef<SWidget> USimpleMinimapWidget::RebuildWidget()
{
	LayerTextures.Reset();
	LayerMaterials.Reset();
	LayerWidgets.Reset();
	LayerInstances.Reset();

	MyOverlay = SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SImage).Image(&Background)
		];
	MyOverlay->RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateUObject(this, &USimpleMinimapWidget::Tick));
	return MyOverlay.ToSharedRef();
}

void USimpleMinimapWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	for (UMaterialInstanceDynamic* MID : LayerMaterials)
	{
		ApplyParameters(MID);
	}
}

void USimpleMinimapWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyOverlay.Reset();
	LayerWidgets.Reset();
}

EActiveTimerReturnType USimpleMinimapWidget::Tick(double CurrentTime, float DeltaTime)
{
	UpdateMarkers();
	return EActiveTimerReturnType::Continue;
}

void USimpleMinimapWidget::ApplyParameters(UMaterialInstanceDynamic* MID) const
{
	if (!MID)
	{
		return;
	}
	MID->SetScalarParameterValue(TEXT("IconSize"), IconSize);
	for (int32 Index = 0; Index < SimpleMinimap::MaterialPaletteSize; ++Index)
	{
		MID->SetVectorParameterValue(*FString::Printf(TEXT("Color%d"), Index),
			Palette.IsValidIndex(Index) ? Palette[Index] : FLinearColor::White);
	}
}

int32 USimpleMinimapWidget::FindOrAddLayer(UTexture* Texture)
{
	const int32 Existing = LayerTextures.Find(Texture);
	if (Existing != INDEX_NONE)
	{
		return Existing;
	}

	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(MarkerMaterial, this);
	MID->SetTextureParameterValue(TEXT("Atlas"), Texture);
	MID->SetVectorParameterValue(TEXT("AtlasSize"), FLinearColor(Texture->GetSurfaceWidth(), Texture->GetSurfaceHeight(), 0.f, 0.f));
	ApplyParameters(MID);

	TSharedRef<SSimpleMinimapLayer> Layer = SNew(SSimpleMinimapLayer).Material(MID);
	MyOverlay->AddSlot()[Layer];

	LayerTextures.Add(Texture);
	LayerMaterials.Add(MID);
	LayerWidgets.Add(Layer);
	return LayerInstances.AddDefaulted();
}

void USimpleMinimapWidget::UpdateMarkers()
{
	const UWorld* World = GetWorld();
	const APlayerController* Player = GetOwningPlayer();
	const USimpleMinimapSubsystem* Subsystem = World ? World->GetSubsystem<USimpleMinimapSubsystem>() : nullptr;
	if (!MyOverlay.IsValid() || !MarkerMaterial || !Player || !Subsystem)
	{
		return;
	}

	// Centre: the pawn (or the camera without one). Up: the view direction, or world +X.
	FVector ViewLocation;
	FRotator ViewRotation;
	Player->GetPlayerViewPoint(ViewLocation, ViewRotation);
	const FVector Center = Player->GetPawn() ? Player->GetPawn()->GetActorLocation() : ViewLocation;
	const float ViewYaw = bRotateWithView ? ViewRotation.Yaw : 0.f;
	float SinView, CosView;
	FMath::SinCos(&SinView, &CosView, FMath::DegreesToRadians(ViewYaw));

	for (FSlateInstanceBufferData& Instances : LayerInstances)
	{
		Instances.Reset();
	}

	const float InvRange = 1.f / FMath::Max(Range, 1.f);
	for (const USimpleMinimapMarkerComponent* Marker : Subsystem->GetMarkers())
	{
		const AActor* Owner = Marker ? Marker->GetOwner() : nullptr;
		UTexture* Texture = nullptr;
		FIntRect RectPx;
		if (!Owner || !Marker->bVisibleOnMinimap || !Marker->ResolveIcon(Texture, RectPx))
		{
			continue;
		}

		// World XY -> radar units: right of the view = +X, ahead = up (-Y on screen).
		const FVector Delta = Owner->GetActorLocation() - Center;
		const float Ahead = Delta.X * CosView + Delta.Y * SinView;
		const float Right = -Delta.X * SinView + Delta.Y * CosView;
		FVector2f Position(Right * InvRange, -Ahead * InvRange);

		// Centre past the edge: hidden, or pinned to the edge. Icons near the edge are cut by the material.
		const float Distance = Position.Size();
		if (Distance > 1.f)
		{
			if (!Marker->bClampToEdge)
			{
				continue;
			}
			Position *= (1.f - IconSize * 0.5f) / Distance;
		}

		const float Angle = (Marker->bRotateWithOwner ? Owner->GetActorRotation().Yaw - ViewYaw : 0.f) + Marker->IconAngleOffset;
		LayerInstances[FindOrAddLayer(Texture)].Add(SimpleMinimap::PackMarker(Position, Angle, Marker->ColorIndex, RectPx));
	}

	for (int32 Index = 0; Index < LayerWidgets.Num(); ++Index)
	{
		LayerWidgets[Index]->SetInstances(MoveTemp(LayerInstances[Index]));
	}
}

#if WITH_EDITOR
const FText USimpleMinimapWidget::GetPaletteCategory()
{
	return LOCTEXT("PaletteCategory", "Minimap");
}
#endif

#undef LOCTEXT_NAMESPACE
