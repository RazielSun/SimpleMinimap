// Demo only: spawns Count actors with a minimap marker and flies them in circles around itself.
// Use several spawners for different groups (ships, static objectives clamped to the edge, ...).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimpleMinimapDemoSpawner.generated.h"

UCLASS()
class SIMPLEMINIMAP_API ASimpleMinimapDemoSpawner : public AActor
{
	GENERATED_BODY()

public:
	ASimpleMinimapDemoSpawner();

	/** Picked in turn for the spawned markers. Sprites of one atlas = one draw for all of them. */
	UPROPERTY(EditAnywhere, Category = "Minimap Demo", meta = (AllowedClasses = "/Script/Engine.Texture2D,/Script/Paper2D.PaperSprite"))
	TArray<TObjectPtr<UObject>> Icons;

	/** Palette indices picked at random; empty = 1..7. */
	UPROPERTY(EditAnywhere, Category = "Minimap Demo")
	TArray<int32> ColorIndices;

	UPROPERTY(EditAnywhere, Category = "Minimap Demo", meta = (ClampMin = "0"))
	int32 Count = 40;

	UPROPERTY(EditAnywhere, Category = "Minimap Demo")
	FFloatInterval OrbitRadius = FFloatInterval(500.f, 6000.f);

	/** Degrees per second; 0 = static markers. */
	UPROPERTY(EditAnywhere, Category = "Minimap Demo")
	FFloatInterval OrbitSpeed = FFloatInterval(5.f, 30.f);

	UPROPERTY(EditAnywhere, Category = "Minimap Demo")
	bool bClampToEdge = false;

	UPROPERTY(EditAnywhere, Category = "Minimap Demo")
	int32 Seed = 1;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

private:
	struct FOrbiter
	{
		TWeakObjectPtr<AActor> Actor;
		float Radius = 0.f;
		float Angle = 0.f;
		float Speed = 0.f;
	};
	TArray<FOrbiter> Orbiters;

	void Place(const FOrbiter& Orbiter) const;
};
