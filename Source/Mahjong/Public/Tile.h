/**
* @author Tim Lake
* @copyright 2026
*/

#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "CoreMinimal.h"
#include "Tile.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTileAdded, const int32&, const FTile&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTileRemoved, const int32&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTileChanged, const int32&, const FTile&);

UENUM(BlueprintType)
enum class ESuit : uint8
{
    None = 0      UMETA(DisplayName = "None"),
    Circle = 1    UMETA(DisplayName = "Circle"),
    Bamboo = 2    UMETA(DisplayName = "Bamboo"),
    Character = 3 UMETA(DisplayName = "Character"),
    Wind = 4      UMETA(DisplayName = "Wind"),
    Dragon = 5    UMETA(DisplayName = "Dragon")
};

USTRUCT(BlueprintType)
struct FTile
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESuit Suit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Number;

    FTile()
    {
        Suit = ESuit::None;
        Number = 0;
    }

    bool operator==(const FTile& Other) const
    {
        return Suit == Other.Suit && Number == Other.Number;
    }

    bool operator!=(const FTile& Other) const
    {
        return Suit != Other.Suit || Number != Other.Number;
    }
};

USTRUCT(BlueprintType)
struct FTileEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTile Tile;
};

USTRUCT(BlueprintType)
struct FTileArray : public FFastArraySerializer
{
    GENERATED_BODY()

    FOnTileAdded OnTileAdded;
    FOnTileRemoved OnTileRemoved;
    FOnTileChanged OnTileChanged;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FTileEntry> Items;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class AMPlayerState* Owner = nullptr;

    int32 FindElemByID(int32 TargetId)
    {
        for (int32 i = 0; i < Items.Num(); ++i) {
            if (Items[i].Tile.Id == TargetId) {
                return i;
            }
        }

        return -1;
    }

    void AddTile(const FTile& Tile)
    {
        FTileEntry& NewItem = Items.AddDefaulted_GetRef();
        NewItem.Tile = Tile;

        MarkItemDirty(NewItem);
    }

    void RemoveTile(int32 Id)
    {
        int32 Index = FindElemByID(Id);
        if (Index < 0) {
            return;
        }

        if (Items.IsValidIndex(Index))
        {
            Items.RemoveAt(Index);
            MarkArrayDirty();
        }
    }

    TArray<FTile> GetTiles()
    {
        TArray<FTile> Tiles;
        for (int32 i = 0; i < Items.Num(); ++i)
        {
            Tiles.Add(Items[i].Tile);
        }
        return Tiles;
    }

    FTile operator[](int Index)
    {
        return Items[Index].Tile;
    }

    int32 Num()
    {
        return Items.Num();
    }

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
    {
        return FFastArraySerializer::FastArrayDeltaSerialize<FTileEntry, FTileArray>(Items, DeltaParms, *this);
    }

    void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
    {
        for (int32 Index : AddedIndices) {
            OnTileAdded.Broadcast(Index, Items[Index].Tile);
        }
    }

    void PostReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
    {
        for (int32 Index : RemovedIndices) {
            OnTileRemoved.Broadcast(Index);
        }
    }

    void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
    {
        for (int32 Index : ChangedIndices) {
            OnTileChanged.Broadcast(Index, Items[Index].Tile);
        }
    }
};

template<>
struct TStructOpsTypeTraits<FTileArray> : public TStructOpsTypeTraitsBase2<FTileArray>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};