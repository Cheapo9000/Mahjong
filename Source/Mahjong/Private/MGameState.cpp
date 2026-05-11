//  /** * @author Tim Lake * @copyright 2026 */

#include "MGameState.h"
#include "Net/UnrealNetwork.h"

void AMGameState::ResetDiscardPile() {
    DiscardPile.Items.Reset();
    DiscardPile.MarkArrayDirty();

    // Broadcast update to players
}

void AMGameState::DisplayDiscardPile() {
    // TODO: Display Discard Pile
}

void AMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMGameState, DiscardPile);
}