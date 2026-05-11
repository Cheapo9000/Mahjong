//  /** * @author Tim Lake * @copyright 2026 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Tile.h"
#include "Deck.h"
#include "Table.h"
#include "MGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MAHJONG_API AMGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	AMGameMode();

	bool CheckWin() const;

	int32 GetMaxPlayers() const;

protected:

	UTable* Table;
};
