/**
* @author Tim Lake
* @copyright 2026
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Tile.h"
#include "MPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MAHJONG_API AMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	UPROPERTY(Replicated)
	TArray<FTile> PrivateHand;

	UPROPERTY(Replicated)
	TArray<FTile> PublicTiles;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
