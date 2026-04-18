/**
* @author Tim Lake
* @copyright 2026
*/

#include "MPlayerState.h"
#include "Net/UnrealNetwork.h"

void AMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AMPlayerState, PrivateHand, COND_OwnerOnly);
    DOREPLIFETIME(AMPlayerState, PublicTiles);
}