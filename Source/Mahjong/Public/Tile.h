/**
* @author Tim Lake
* @copyright 2026
*/

#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "CoreMinimal.h"
#include "Tile.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTileAdded, int32, const FTile&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTileRemoved, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTileChanged, int32, const FTile&);

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

    UPROPERTY()
    ESuit Suit;

    UPROPERTY()
    int32 Number;
};

USTRUCT()
struct FTileEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY()
    FTile Tile;
};

USTRUCT()
struct FTileArray : public FFastArraySerializer
{
    GENERATED_BODY()

    FOnTileAdded OnTileAdded;
    FOnTileRemoved OnTileRemoved;
    FOnTileChanged OnTileChanged;

    UPROPERTY()
    TArray<FTileEntry> Items;

    class AMyPlayerState* Owner = nullptr;

    void AddTile(const FTile& Tile)
    {
        FTileEntry& NewItem = Items.AddDefaulted_GetRef();
        NewItem.Tile = Tile;

        MarkItemDirty(NewItem);
    }

    void RemoveTile(int32 Index)
    {
        if (Items.IsValidIndex(Index))
        {
            Items.RemoveAt(Index);
            MarkArrayDirty();
        }
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