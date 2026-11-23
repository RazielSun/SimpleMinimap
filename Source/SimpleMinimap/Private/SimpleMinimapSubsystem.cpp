#include "SimpleMinimapSubsystem.h"

#include "SimpleMinimapMarkerComponent.h"

void USimpleMinimapSubsystem::Register(USimpleMinimapMarkerComponent* Marker)
{
	if (Marker)
	{
		Markers.AddUnique(Marker);
	}
}

void USimpleMinimapSubsystem::Unregister(USimpleMinimapMarkerComponent* Marker)
{
	Markers.RemoveSingleSwap(Marker);
}
