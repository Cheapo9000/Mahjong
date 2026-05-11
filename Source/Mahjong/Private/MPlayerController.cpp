/**
* @author Tim Lake
* @copyright 2026
*/

#include "MPlayerController.h"
#include "MGameState.h"
#include "MPlayerState.h"

void AMPlayerController::BeginPlay() {
    Super::BeginPlay();

    if (HasAuthority()) {
        InitializeFromPlayerState(GetPlayerState<AMPlayerState>());
    }
}

void AMPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
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
        // Update UI
    }
}

void AMPlayerController::HandlePlayerSeatChanged(const int32& NewSeat)
{
    UE_LOG(LogTemp, Warning, TEXT("Seat changed: %d"), NewSeat);

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        // Update UI
    }
}

void AMPlayerController::HandlePlayerHandChanged(const FTileArray& NewHand)
{
    UE_LOG(LogTemp, Warning, TEXT("Player hand changed"));

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        // Update UI
    }
}

void AMPlayerController::HandlePlayerDrewFromDiscard(const FTile& Tile)
{
    UE_LOG(LogTemp, Warning, TEXT("Player drew a tile from the discard pile: %d"), Tile.Id);

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        // Update UI
    }
}

void AMPlayerController::HandlePlayerDrewTile()
{
    UE_LOG(LogTemp, Warning, TEXT("Player drew a tile."));

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        // Update UI
    }
}

void AMPlayerController::HandlePlayerDiscard(const FTile& Tile)
{
    UE_LOG(LogTemp, Warning, TEXT("Player discarded a tile: %d"), Tile.Id);

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        // Update UI
    }
}

void AMPlayerController::HandleOnTileAdded(const int32& Index, const FTile& Tile)
{
    UE_LOG(LogTemp, Warning, TEXT("Tile added: %d"), Index);

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        // Update UI
    }
}

void AMPlayerController::HandleOnTileRemoved(const int32& Index)
{
    UE_LOG(LogTemp, Warning, TEXT("Tile removed: %d"), Index);

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        // Update UI
    }
}

void AMPlayerController::HandleOnTileChanged(const int32& Index, const FTile& Tile)
{
    UE_LOG(LogTemp, Warning, TEXT("Tile chaged: %d"), Index);

    AMPlayerState* PS = GetPlayerState<AMPlayerState>();
    if (PS)
    {
        // Update UI
    }
}

int32 AMPlayerController::FindElemById(const int32& Id) const
{
    AMGameState* GS = GetWorld()->GetGameState<AMGameState>();
    if (GS)
    {
        for (int32 i = 0; i < GS->PlayerArray.Num(); ++i) {
            AMPlayerState* PS = Cast<AMPlayerState>(GS->PlayerArray[i]);
            if (PS)
            {
                int32 SeatPosition = PS->Seat;
                if (SeatPosition == Id)
                {
                    return SeatPosition;
                }
            }
        }
    }
    return -1;
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
    PS->OnPlayerHandChanged.AddDynamic(this, &AMPlayerController::HandlePlayerHandChanged);
    PS->OnPlayerDrewFromDiscard.AddDynamic(this, &AMPlayerController::HandlePlayerDrewFromDiscard);
    PS->OnPlayerDrewTile.AddDynamic(this, &AMPlayerController::HandlePlayerDrewTile);
    PS->OnPlayerDiscard.AddDynamic(this, &AMPlayerController::HandlePlayerDiscard);
    PS->PrivateHand.OnTileAdded.AddUObject(this, &AMPlayerController::HandleOnTileAdded);
    PS->PrivateHand.OnTileRemoved.AddUObject(this, &AMPlayerController::HandleOnTileRemoved);
    PS->PrivateHand.OnTileChanged.AddUObject(this, &AMPlayerController::HandleOnTileChanged);
    PS->PublicTiles.OnTileAdded.AddUObject(this, &AMPlayerController::HandleOnTileAdded);
    PS->PublicTiles.OnTileRemoved.AddUObject(this, &AMPlayerController::HandleOnTileRemoved);
    PS->PublicTiles.OnTileChanged.AddUObject(this, &AMPlayerController::HandleOnTileChanged);
}