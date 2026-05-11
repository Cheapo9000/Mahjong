#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "CoreMinimal.h"
#include "Deck.h"
#include "Tile.h"
#include "MPlayerController.h"
#include "Table.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UTable : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 MaxPlayers = 4;

	UTable();

	UFUNCTION()
	int32 SeatPlayer(const FString& Name);

	UFUNCTION()
	void RemovePlayer(const int32& SeatPosition);

	UFUNCTION()
	void DealHand(const int32& SeatPosition);

	UFUNCTION()
	void DealHands();

	UFUNCTION()
	FTile DrawTile();

	UFUNCTION()
	void DiscardDrawn();

	UFUNCTION()
	void KeepDrawn();

	UFUNCTION()
	bool DiscardTile(const int32& Id);

	UFUNCTION()
	void SetCurrentPlayer(const int32& Seat, bool OverritePrevious = false);
	
	UFUNCTION()
	void NextPlayer();

	UFUNCTION()
	void SetWantsPeng(const int32 Seat);

	UFUNCTION()
	void WelcomePLayers() const;

	UFUNCTION()
	void DisplayTable(const int32& SeatPosition) const;

	UFUNCTION()
	bool CanChi() const;

	UFUNCTION()
	void Chi();

	UFUNCTION()
	EFlags CanPengOrGong(const int32& SeatPosition) const;

	UFUNCTION()
	EFlags CanPengOrGongAll() const;

	UFUNCTION()
	EFlags PengOrGong(const int32& SeatPosition);

	UFUNCTION()
	void DisplayPengOrGongPlayers() const;

	UFUNCTION()
	void RevealSet(FTileArray& Hand, FTileArray& PublicHand);

	UFUNCTION()
	bool CheckWin() const;

	UFUNCTION()
	FTile GetDrawnTile() const;

	UFUNCTION()
	AMPlayerController* GetPlayer(const int32& SeatPosition);

	UFUNCTION()
	TArray<FTile> GetPlayerHand(const int32& SeatPosition);

	UFUNCTION()
	UDeck* GetDeck();

	UFUNCTION()
	int32 GetCurrentPlayer() const;

private:

	TArray<AMPlayerController*> Players;

	UDeck* Deck;

	FTile DrawnTile;

	bool FromDiscardPile;

	int32 PreviousPlayer;

	int32 CurrentPlayer;

	int32 PlayerCount;
};