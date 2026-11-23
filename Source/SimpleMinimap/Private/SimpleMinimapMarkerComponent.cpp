#include "SimpleMinimapMarkerComponent.h"

#include "Engine/Texture.h"
#include "Engine/World.h"
#include "SimpleMinimapSubsystem.h"
#include "Slate/SlateTextureAtlasInterface.h"

void USimpleMinimapMarkerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (USimpleMinimapSubsystem* Subsystem = UWorld::GetSubsystem<USimpleMinimapSubsystem>(GetWorld()))
	{
		Subsystem->Register(this);
	}
}

void USimpleMinimapMarkerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (USimpleMinimapSubsystem* Subsystem = UWorld::GetSubsystem<USimpleMinimapSubsystem>(GetWorld()))
	{
		Subsystem->Unregister(this);
	}
	Super::EndPlay(EndPlayReason);
}

void USimpleMinimapMarkerComponent::SetIcon(UObject* NewIcon)
{
	Icon = NewIcon;
}

bool USimpleMinimapMarkerComponent::ResolveIcon(UTexture*& OutTexture, FIntRect& OutRectPx) const
{
	OutTexture = nullptr;
	if (UTexture* Texture = Cast<UTexture>(Icon))
	{
		OutTexture = Texture;
		OutRectPx = FIntRect(0, 0, FMath::RoundToInt(Texture->GetSurfaceWidth()), FMath::RoundToInt(Texture->GetSurfaceHeight()));
	}
	else if (const ISlateTextureAtlasInterface* Atlas = Cast<ISlateTextureAtlasInterface>(Icon))
	{
		// Paper2D sprites and other atlas entries: rect from the baked atlas data, no Paper2D dependency.
		const FSlateAtlasData Data = Atlas->GetSlateAtlasData();
		if (Data.AtlasTexture)
		{
			OutTexture = Data.AtlasTexture;
			const FVector2D Size(Data.AtlasTexture->GetSurfaceWidth(), Data.AtlasTexture->GetSurfaceHeight());
			const FIntPoint Min((Data.StartUV * Size).IntPoint());
			OutRectPx = FIntRect(Min, Min + FIntPoint((Data.SizeUV * Size).IntPoint()));
		}
	}
	return OutTexture != nullptr && OutRectPx.Area() > 0;
}
