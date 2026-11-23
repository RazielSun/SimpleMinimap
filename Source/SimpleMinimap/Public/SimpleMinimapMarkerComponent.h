// Put on any actor to show it on every SimpleMinimap widget of its world. Setup only: pick an icon,
// a palette colour and whether the icon turns with the actor.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SimpleMinimapMarkerComponent.generated.h"

class UTexture;

UCLASS(ClassGroup = (UI), meta = (BlueprintSpawnableComponent))
class SIMPLEMINIMAP_API USimpleMinimapMarkerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** A Texture2D (whole texture) or a sprite from an atlas (anything implementing SlateTextureAtlasInterface,
	 *  e.g. a Paper2D sprite). Markers that share one texture are drawn in one batch. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap", meta = (AllowedClasses = "/Script/Engine.Texture2D,/Script/Paper2D.PaperSprite"))
	TObjectPtr<UObject> Icon;

	/** Index into the widget's Palette. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap", meta = (ClampMin = "0", ClampMax = "7"))
	int32 ColorIndex = 0;

	/** Turn the icon with the actor's yaw. Off: the icon always points up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	bool bRotateWithOwner = true;

	/** Added to the icon angle, degrees clockwise -- for art that does not point up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float IconAngleOffset = 0.f;

	/** Keep the icon on the radar edge when the actor is out of range (objectives); otherwise hidden. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	bool bClampToEdge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	bool bVisibleOnMinimap = true;

	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void SetIcon(UObject* NewIcon);

	/** The texture to draw from and the icon's rect in it, in pixels. False if Icon is not usable. */
	bool ResolveIcon(UTexture*& OutTexture, FIntRect& OutRectPx) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
