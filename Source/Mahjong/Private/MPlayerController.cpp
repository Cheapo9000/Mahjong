/**
* @author Tim Lake
* @copyright 2026
*/

#include "MPlayerController.h"

void AMPlayerController::resetTile(FTile& tile) {
	tile.Suit = ESuit::None;
	tile.Number = 0;
}