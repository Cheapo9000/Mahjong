#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "CoreMinimal.h"
#include "Tile.h"
#include "MPlayerController.h"
#include "Deck.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UDeck : public UObject
{
	GENERATED_BODY()

public:

	UDeck();

	void Reset(TArray<AMPlayerController*>& Players, FTileArray& DiscardPile);

	void AddTiles(TArray<FTile>& NewTiles);

	void SetTiles(TArray<FTile>& NewTiles);

	int32 GetHandSize() const;

	FTileArray* GetTiles();

	TArray<FTile> GetHand();

	FTile DrawTile();

private:

	int32 MaxTiles;

	int32 HandSize;

	FTileArray Tiles;

	void Build();
};