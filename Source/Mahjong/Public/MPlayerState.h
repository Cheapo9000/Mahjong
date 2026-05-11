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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSeated, const int32&, PlayerId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStood, const int32&, PlayerId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHandChanged, const FTileArray&, NewHand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDrewFromDiscard, const FTile&, Tile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDrewTile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDiscard, const FTile&, Tile);

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesInBlueprints = "true"))
enum class EFlags : uint8
{
	None = 0			UMETA(DisplayName = "None"),
	Seated = 1 << 0		UMETA(DisplayName = "Seated"),
	CanPoG = 1 << 1		UMETA(DisplayName = "CanPengOrGong"),
	Peng = 1 << 2		UMETA(DisplayName = "Peng"),
	Gong = 1 << 3		UMETA(DisplayName = "Gong"),
	WantsPoG = 1 << 4	UMETA(DisplayName = "WantsPengOrGong")
};

ENUM_CLASS_FLAGS(EFlags);

/**
 * 
 */
UCLASS()
class MAHJONG_API AMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	// Do no auto repicate
	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "/Script/Mahjong.EFlags"))
	EFlags OldFlags;

	UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "/Script/Mahjong.EFlags"), ReplicatedUsing = OnRep_FlagUpdate)
	EFlags Flags;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SeatPosition)
	int32 Seat;

	UPROPERTY(Replicated)
	FTileArray PrivateHand;

	UPROPERTY(ReplicatedUsing = OnRep_Hand)
	FTileArray PublicTiles;

	UPROPERTY(ReplicatedUsing = OnRep_TileDrawn)
	FTile DrawnTile;

	UPROPERTY(ReplicatedUsing = OnRep_TileDrawn)
	FTile DiscardDrawnTile;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerNameChanged OnPlayerNameChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerSeatPositionChanged OnPlayerSeatPositionChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerSeated OnPlayerSeated;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStood OnPlayerStood;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerHandChanged OnPlayerHandChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDrewFromDiscard OnPlayerDrewFromDiscard;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDrewTile OnPlayerDrewTile;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDiscard OnPlayerDiscard;

	void SetName(const FString& NewName);

	void SetPlayerSeatPosition(const int32& seatPosition);

	void SetPlayerHand(const TArray<FTile>& Hand);

	void Reset();

protected:

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerName(const FString& NewName);

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerSeatPosition(const int32& NewSeat);

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerHand(const TArray<FTile>& Hand);

	UFUNCTION(Server, Reliable)
	void Server_Reset();

	void OnRep_PlayerName() override;

	UFUNCTION()
	void OnRep_FlagUpdate();

	UFUNCTION()
	void OnRep_SeatPosition();

	UFUNCTION()
	void OnRep_Hand();

	UFUNCTION()
	void OnRep_TileDrawn();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
