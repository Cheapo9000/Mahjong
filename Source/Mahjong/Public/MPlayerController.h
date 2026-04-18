/**
* @author Tim Lake
* @copyright 2026
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Tile.h"
#include "MPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MAHJONG_API AMPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	UFUNCTION()
	void resetTile(FTile& tile);

};
