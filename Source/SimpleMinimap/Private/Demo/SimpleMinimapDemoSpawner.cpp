#include "Demo/SimpleMinimapDemoSpawner.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "SimpleMinimapMarkerComponent.h"

ASimpleMinimapDemoSpawner::ASimpleMinimapDemoSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ASimpleMinimapDemoSpawner::BeginPlay()
{
	Super::BeginPlay();

	FRandomStream Random(Seed);
	for (int32 Index = 0; Index < Count && Icons.Num() > 0; ++Index)
	{
		AActor* Actor = GetWorld()->SpawnActor<AActor>();
		USceneComponent* Root = NewObject<USceneComponent>(Actor, TEXT("Root"));
		Actor->SetRootComponent(Root);
		Root->RegisterComponent();

		USimpleMinimapMarkerComponent* Marker = NewObject<USimpleMinimapMarkerComponent>(Actor, TEXT("MinimapMarker"));
		Marker->Icon = Icons[Index % Icons.Num()];
		Marker->ColorIndex = ColorIndices.Num() > 0 ? ColorIndices[Random.RandHelper(ColorIndices.Num())] : Random.RandRange(1, 7);
		Marker->bClampToEdge = bClampToEdge;
		Marker->RegisterComponent();

		FOrbiter& Orbiter = Orbiters.AddDefaulted_GetRef();
		Orbiter.Actor = Actor;
		// sqrt: uniform over the disc, not bunched at the centre.
		Orbiter.Radius = FMath::Lerp(OrbitRadius.Min, OrbitRadius.Max, FMath::Sqrt(Random.FRand()));
		Orbiter.Angle = Random.FRandRange(0.f, 360.f);
		Orbiter.Speed = Random.FRandRange(OrbitSpeed.Min, OrbitSpeed.Max) * (Random.FRand() < 0.5f ? -1.f : 1.f);
		Place(Orbiter);
	}
}

void ASimpleMinimapDemoSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	for (FOrbiter& Orbiter : Orbiters)
	{
		if (Orbiter.Speed != 0.f)
		{
			Orbiter.Angle = FMath::Fmod(Orbiter.Angle + Orbiter.Speed * DeltaSeconds, 360.f);
			Place(Orbiter);
		}
	}
}

void ASimpleMinimapDemoSpawner::Place(const FOrbiter& Orbiter) const
{
	if (AActor* Actor = Orbiter.Actor.Get())
	{
		const float Radians = FMath::DegreesToRadians(Orbiter.Angle);
		const FVector Offset(FMath::Cos(Radians) * Orbiter.Radius, FMath::Sin(Radians) * Orbiter.Radius, 0.f);
		// Face along the orbit: +90 deg when the angle grows, -90 when it shrinks.
		const float Yaw = Orbiter.Angle + (Orbiter.Speed >= 0.f ? 90.f : -90.f);
		Actor->SetActorLocationAndRotation(GetActorLocation() + Offset, FRotator(0.f, Yaw, 0.f));
	}
}
