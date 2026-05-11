//  /** * @author Tim Lake * @copyright 2026 */


#include "MGameMode.h"
#include "MPlayerController.h"
#include "MPlayerState.h"

AMGameMode::AMGameMode()
{
	Table = NewObject<UTable>();
	//PlayerControllerClass = AMPlayerController::StaticClass();
	//PlayerStateClass = AMPlayerState::StaticClass();
}

bool AMGameMode::CheckWin() const {
	if (Table)
	{
		return Table->CheckWin();
	}
	return false;
}

int32 AMGameMode::GetMaxPlayers() const {
	if (Table)
	{
		return Table->MaxPlayers;
	}
	return 0;
}