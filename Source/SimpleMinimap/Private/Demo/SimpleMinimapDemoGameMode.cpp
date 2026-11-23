#include "Demo/SimpleMinimapDemoGameMode.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "SimpleMinimapMarkerComponent.h"

void ASimpleMinimapDemoGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* Player = GetWorld()->GetFirstPlayerController(); Player && WidgetClass)
	{
		if (UUserWidget* Widget = CreateWidget<UUserWidget>(Player, WidgetClass))
		{
			Widget->AddToViewport();
		}
	}
}

void ASimpleMinimapDemoGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
	APawn* Pawn = NewPlayer ? NewPlayer->GetPawn() : nullptr;
	if (!Pawn || !PlayerIcon || Pawn->FindComponentByClass<USimpleMinimapMarkerComponent>())
	{
		return;
	}
	USimpleMinimapMarkerComponent* Marker = NewObject<USimpleMinimapMarkerComponent>(Pawn, TEXT("MinimapMarker"));
	Marker->Icon = PlayerIcon;
	Marker->ColorIndex = 0;
	Marker->RegisterComponent();
}
