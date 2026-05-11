#include "Deck.h"
#include "CoreMinimal.h"
#include "MGameState.h"
#include "Tile.h"
#include "MPlayerController.h"
#include "Net/UnrealNetwork.h"

UDeck::UDeck() {
    HandSize = 13;
    MaxTiles = 136;
    this->Build();
}

void UDeck::Reset(TArray<AMPlayerController*>& Players, FTileArray& DiscardPile) {
    AMPlayerController* ValidPC = nullptr;
    for (int32 i = 0; i < Players.Num(); ++i) {
        AMPlayerController* PC = Players[i];
        if (!PC)
        {
            continue;
        }
        ValidPC = PC;
        AMPlayerState* PS = ((AMPlayerState*)PC->PlayerState);
        if (!PS)
        {
            continue;
        }

        for (int32 j = 0; j < PS->PrivateHand.Items.Num(); ++j) {
            Tiles.AddTile(PS->PrivateHand.Items[j].Tile);
        }
        for (int32 j = 0; j < PS->PublicTiles.Items.Num(); ++j) {
            Tiles.AddTile(PS->PublicTiles.Items[j].Tile);
        }

        PS->Reset();
    }
    for (int32 i = 0; i < DiscardPile.Items.Num(); ++i) {
        Tiles.AddTile(DiscardPile.Items[i].Tile);
    }

    if (ValidPC) 
    {
        AMGameState* GS = ValidPC->GetWorld()->GetGameState<AMGameState>();
        if (GS)
        {
            GS->ResetDiscardPile();
        }
    }
}

void UDeck::AddTiles(TArray<FTile>& NewTiles) {
    for (int32 i = 0; i < NewTiles.Num(); ++i) {
        Tiles.AddTile(NewTiles[i]);
    }
}

void UDeck::SetTiles(TArray<FTile>& NewTiles) {
    Tiles.Items.Reset();
    AddTiles(NewTiles);
}

int32 UDeck::GetHandSize() const {
    return HandSize;
}

FTileArray* UDeck::GetTiles() {
    return &Tiles;
}

TArray<FTile> UDeck::GetHand() {
    TArray<FTile> Hand;
    int32 Index;

    for (int32 i = 0; i < HandSize; ++i) {
        Index = FMath::RandRange(0, Tiles.Items.Num() - 1);
        Hand.Add(Tiles.Items[Index].Tile);
        Tiles.Items.RemoveAtSwap(Index);
    }

    return Hand;
}

FTile UDeck::DrawTile() {
    int32 Index = FMath::RandRange(0, Tiles.Items.Num() - 1);
    FTile Drawn = Tiles.Items[Index].Tile;
    Tiles.Items.RemoveAtSwap(Index);
    return Drawn;
}

void UDeck::Build() {
    Tiles.Items.Reset();

    int32 Id = 0;
	int32 NumMax = 9;
    FTile Tile;
    for (int32 Kind = 1; (int32)Kind <= (int32)ESuit::Dragon; ++Kind) {
        if ((int32)Kind == (int32)ESuit::Wind) {
            NumMax = 4;
        }
        else if ((int32)Kind == (int32)ESuit::Dragon) {
            NumMax = 3;
        }
        for (int32 Num = 1; Num <= NumMax; ++Num) {
            for (int32 Dup = 0; Dup < 4; ++Dup) {
                Tile.Id = Id;
                Tile.Suit = (ESuit)Kind;
                Tile.Number = Num;
                Tiles.AddTile(Tile);
                ++Id;
            }
        }
    }
    // Enforce the correct tile count
    if (Tiles.Items.Num() != MaxTiles) {
        Tiles.Items.Reset();
    }
}