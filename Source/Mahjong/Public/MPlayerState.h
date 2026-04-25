/**
* @author Tim Lake
* @copyright 2026
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Tile.h"
#include "MPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerNameChanged, const FString&, NewName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSeatPositionChanged, const int32&, NewSeat);

/**
 * 
 */
UCLASS()
class MAHJONG_API AMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Name)
	FString Name;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SeatPosition)
	int32 Seat;

	UPROPERTY(ReplicatedUsing = OnRep_Hand)
	FTileArray PrivateHand;

	UPROPERTY(Replicated)
	FTileArray PublicTiles;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerNameChanged OnPlayerNameChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerSeatPositionChanged OnPlayerSeatPositionChanged;

	void SetPlayerName(const FString& NewName);

	void SetPlayerSeatPosition(const int32& seatPosition);

	void SetPlayerHand(const TArray<FTile>& Hand);

	void Reset();

	//UFUNCTION()
	void HandleNameChanged();

	//UFUNCTION()
	void HandleSeatPositionChanged();

protected:

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerName(const FString& NewName);

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerSeatPosition(const int32& NewSeat);

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerHand(const TArray<FTile>& Hand);

	UFUNCTION(Server, Reliable)
	void Server_Reset();

	UFUNCTION()
	void OnRep_Name();

	UFUNCTION()
	void OnRep_SeatPosition();

	UFUNCTION()
	void OnRep_Hand();

	UFUNCTION()
	void HandleHandChanged();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
