/**
* @author Tim Lake
* @copyright 2026
*/

#include "MPlayerController.h"
#include "MPlayerState.h"

void AMPlayerController::BeginPlay() {
    Super::BeginPlay();

    if (HasAuthority()) {
        InitializeFromPlayerState(GetPlayerState<AMPlayerState>());
    }
}

void AMPlayerController::Server_RequestNameChange_Implementation(const FString & NewName)
{
    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        PS->SetPlayerName(NewName);
    }
}

void AMPlayerController::Server_RequestSeatChange_Implementation(const int32& NewSeat)
{
    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        PS->SetPlayerSeatPosition(NewSeat);
    }
}

void AMPlayerController::resetTile(FTile& tile) {
	tile.Suit = ESuit::None;
	tile.Number = 0;
}

void AMPlayerController::HandlePlayerNameChanged(const FString& NewName)
{
    UE_LOG(LogTemp, Warning, TEXT("Name changed: %s"), *NewName);

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        PS->HandleNameChanged();
    }
}

void AMPlayerController::HandlePlayerSeatChanged(const int32& NewSeat)
{
    UE_LOG(LogTemp, Warning, TEXT("Seat changed: %d"), NewSeat);

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        PS->HandleSeatPositionChanged();
    }
}

void AMPlayerController::OnRep_PlayerState() {
    Super::OnRep_PlayerState();

    InitializeFromPlayerState(GetPlayerState<AMPlayerState>());
}

void AMPlayerController::InitializeFromPlayerState(AMPlayerState* PS) {
    if (!PS) {
        return;
    }

    PS->OnPlayerNameChanged.AddDynamic(this, &AMPlayerController::HandlePlayerNameChanged);
    PS->OnPlayerSeatPositionChanged.AddDynamic(this, &AMPlayerController::HandlePlayerSeatChanged);
}