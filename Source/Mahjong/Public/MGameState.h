//  /** * @author Tim Lake * @copyright 2026 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Tile.h"
#include "MGameState.generated.h"

/**
 * 
 */
UCLASS()
class MAHJONG_API AMGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Replicated)
	FTileArray DiscardPile;

	void ResetDiscardPile();

	void DisplayDiscardPile();

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
