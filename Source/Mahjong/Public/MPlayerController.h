/**
* @author Tim Lake
* @copyright 2026
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Tile.h"
#include "MPlayerState.h"
#include "MPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MAHJONG_API AMPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void BeginPlay() override;
	
	UFUNCTION(Server, Reliable)
	void Server_RequestNameChange(const FString& NewName);

	UFUNCTION()
	void resetTile(FTile& tile);

	UFUNCTION()
	void HandlePlayerNameChanged(const FString& NewName);

protected:
	void OnRep_PlayerState();

	void InitializeFromPlayerState(AMPlayerState* PS);
};
