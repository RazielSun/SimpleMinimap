// Registry of the minimap markers in a world. Components add themselves on BeginPlay; widgets read the list.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SimpleMinimapSubsystem.generated.h"

class USimpleMinimapMarkerComponent;

UCLASS()
class SIMPLEMINIMAP_API USimpleMinimapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void Register(USimpleMinimapMarkerComponent* Marker);
	void Unregister(USimpleMinimapMarkerComponent* Marker);

	TConstArrayView<TObjectPtr<USimpleMinimapMarkerComponent>> GetMarkers() const { return Markers; }

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<USimpleMinimapMarkerComponent>> Markers;
};
