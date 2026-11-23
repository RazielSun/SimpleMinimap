// Radar minimap for UMG: drop it into any Widget Blueprint (keep it square, e.g. in a Size Box).
// Every frame it reads the world's USimpleMinimapMarkerComponents, turns them into radar positions
// around the owning player, packs one float4 per marker and draws one batch per icon texture
// (SSimpleMinimapLayer). Setup is all properties; there is no logic to write in Blueprint.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Styling/SlateBrush.h"
#include "SimpleMinimapWidget.generated.h"

class SOverlay;
class SSimpleMinimapLayer;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UTexture;

UCLASS()
class SIMPLEMINIMAP_API USimpleMinimapWidget : public UWidget
{
	GENERATED_BODY()

public:
	USimpleMinimapWidget();

	/** Radar background, drawn under the markers (M_SimpleMinimapRadar or any brush). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	FSlateBrush Background;

	/** Marker material (M_SimpleMinimapMarker). One dynamic instance is made per icon texture. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UMaterialInterface> MarkerMaterial;

	/** World distance shown from the centre to the radar edge, in Unreal units. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap", meta = (ClampMin = "1"))
	float Range = 5000.f;

	/** On: the view direction is up. Off: world +X is up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	bool bRotateWithView = true;

	/** Longest side of an icon, as a fraction of the radar radius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap", meta = (ClampMin = "0.01", ClampMax = "1"))
	float IconSize = 0.1f;

	/** Tints picked by USimpleMinimapMarkerComponent::ColorIndex. The material has 8 slots. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap", EditFixedSize)
	TArray<FLinearColor> Palette;

	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	EActiveTimerReturnType Tick(double CurrentTime, float DeltaTime);
	void UpdateMarkers();
	int32 FindOrAddLayer(UTexture* Texture);
	void ApplyParameters(UMaterialInstanceDynamic* MID) const;

	// One entry per icon texture, same index in all three.
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTexture>> LayerTextures;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> LayerMaterials;
	TArray<TSharedPtr<SSimpleMinimapLayer>> LayerWidgets;
	TArray<FSlateInstanceBufferData> LayerInstances;

	TSharedPtr<SOverlay> MyOverlay;
};
