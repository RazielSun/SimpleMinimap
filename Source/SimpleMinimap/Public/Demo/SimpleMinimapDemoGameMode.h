// Demo only: shows WidgetClass and gives the player's pawn a minimap marker.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SimpleMinimapDemoGameMode.generated.h"

class UUserWidget;

UCLASS()
class SIMPLEMINIMAP_API ASimpleMinimapDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Widget added to the viewport on BeginPlay (WBP_SimpleMinimapDemo). */
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Demo")
	TSubclassOf<UUserWidget> WidgetClass;

	/** Icon of the player's own marker; a single texture in the demo. */
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Demo", meta = (AllowedClasses = "/Script/Engine.Texture2D,/Script/Paper2D.PaperSprite"))
	TObjectPtr<UObject> PlayerIcon;

	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;
};
