/**
* @author Tim Lake
* @copyright 2026
*/

#pragma once

#include "CoreMinimal.h"
#include "Tile.generated.h"

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

USTRUCT()
struct FTile
{
    GENERATED_BODY()

    UPROPERTY()
    ESuit Suit;

    UPROPERTY()
    int32 Number;
};