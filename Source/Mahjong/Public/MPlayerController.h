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
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void SetupInputComponent() override;
	
	UFUNCTION(Server, Reliable)
	void Server_RequestNameChange(const FString& NewName);

	UFUNCTION(Server, Reliable)
	void Server_RequestSeatChange(const int32& NewSeat);

	UFUNCTION()
	void resetTile(FTile& tile);

	UFUNCTION()
	void HandlePlayerNameChanged(const FString& NewName);

	UFUNCTION()
	void HandlePlayerSeatChanged(const int32& NewSeat);

	UFUNCTION()
	void HandlePlayerHandChanged(const FTileArray& NewHand);

	UFUNCTION()
	void HandlePlayerDrewFromDiscard(const FTile& Tile);

	UFUNCTION()
	void HandlePlayerDrewTile();

	UFUNCTION()
	void HandlePlayerDiscard(const FTile& Tile);

	UFUNCTION()
	void HandleOnTileAdded(const int32& Index, const FTile& Tile);

	UFUNCTION()
	void HandleOnTileRemoved(const int32& Index);

	UFUNCTION()
	void HandleOnTileChanged(const int32& Index, const FTile& Tile);

	UFUNCTION()
	int32 FindElemById(const int32& Id) const;

protected:
	void OnRep_PlayerState();

	void InitializeFromPlayerState(AMPlayerState* PS);
};
