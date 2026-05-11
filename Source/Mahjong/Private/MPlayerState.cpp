/**
* @author Tim Lake
* @copyright 2026
*/

#include "MPlayerState.h"
#include "MGameMode.h"
#include "MGameState.h"
#include "Net/UnrealNetwork.h"

void AMPlayerState::SetName(const FString& NewName)
{
    if (HasAuthority())
    {
        // We are the server
        SetPlayerName(NewName);
        OnPlayerNameChanged.Broadcast(NewName);
    }
    else
    {
        // We are a client → ask server
        Server_SetPlayerName(NewName);
    }
}

void AMPlayerState::SetPlayerSeatPosition(const int32& NewSeat)
{
    if (HasAuthority())
    {
        AMGameMode* GM = Cast<AMGameMode>(GetWorld()->GetAuthGameMode());

        if (!GM || NewSeat > GM->GetMaxPlayers())
        {
            return;
        }

        // We are the server
        OldFlags = Flags;
        if (NewSeat < 0) 
        {
            Flags &= ~EFlags::Seated;
        }
        else 
        {
            Flags |= EFlags::Seated;
        }
        Seat = NewSeat;
        OnPlayerSeatPositionChanged.Broadcast(Seat);
    }
    else
    {
        // We are a client → ask server
        Server_SetPlayerSeatPosition(NewSeat);
    }
}

void AMPlayerState::SetPlayerHand(const TArray<FTile>& Hand)
{
    if (HasAuthority())
    {
        // We are the server
        PrivateHand.Items.Reset();

        for (const FTile& Tile : Hand)
        {
            PrivateHand.AddTile(Tile);
        }

        PrivateHand.MarkArrayDirty();

        OnPlayerHandChanged.Broadcast(PublicTiles);
    }
    else
    {
        // We are a client → ask server
        Server_SetPlayerHand(Hand);
    }
}

void AMPlayerState::Reset()
{
    if (HasAuthority())
    {
        // We are the server
        PrivateHand.Items.Reset();
        PublicTiles.Items.Reset();
        SetPlayerName(TEXT(""));
        SetPlayerSeatPosition(-1);

        PrivateHand.MarkArrayDirty();
        PublicTiles.MarkArrayDirty();

        OnPlayerHandChanged.Broadcast(PublicTiles);
    }
    else
    {
        // We are a client → ask server
        Server_Reset();
    }
}

void AMPlayerState::Server_SetPlayerName_Implementation(const FString& NewName)
{
    SetName(NewName);
}

void AMPlayerState::Server_SetPlayerSeatPosition_Implementation(const int32& NewSeat)
{
    SetPlayerSeatPosition(NewSeat);
}

void AMPlayerState::Server_SetPlayerHand_Implementation(const TArray<FTile>& Hand)
{
    SetPlayerHand(Hand);
}

void AMPlayerState::Server_Reset_Implementation()
{
    Reset();
}

void AMPlayerState::OnRep_PlayerName()
{
    Super::OnRep_PlayerName();

    UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerName called!"));
    // Called on clients when PlayerName updates
    OnPlayerNameChanged.Broadcast(GetPlayerName());
}

void AMPlayerState::OnRep_FlagUpdate()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_FlagUpdate called!"));

    //EFlags : uint8
    //None = 0			    UMETA(DisplayName = "None"),
    //Seated = 1 << 0		UMETA(DisplayName = "Seated"),
    //CanPoG = 1 << 1		UMETA(DisplayName = "CanPengOrGong"),
    //PengOGong = 1 << 2	UMETA(DisplayName = "Peng"),
    //Gong = 1 << 3		    UMETA(DisplayName = "Gong"),
    //WantsPoG = 1 << 4	    UMETA(DisplayName = "WantsPengOrGong")
 
    // Called on clients when Player seat updates
    if ((Flags & EFlags::Seated) != (OldFlags & EFlags::Seated)) {
        if ((Flags & EFlags::Seated) == EFlags::Seated) {
            OnPlayerSeated.Broadcast(Seat);
        }
        else {
            OnPlayerStood.Broadcast(Seat);
        }
    }

    OldFlags = Flags;
}

void AMPlayerState::OnRep_SeatPosition()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_SeatPosition called!"));
    // Called on clients when PlayerSeatPosition updates
    OnPlayerSeatPositionChanged.Broadcast(Seat);
}

void AMPlayerState::OnRep_Hand()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_Hand called!"));
    // Called on clients when PlayerHand updates
    OnPlayerHandChanged.Broadcast(PublicTiles);
}

void AMPlayerState::OnRep_TileDrawn()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_TileDrawn called!"));

    // Called on clients when DrawnTile updates
    if (DrawnTile.Number > 0) {
        UE_LOG(LogTemp, Warning, TEXT("Tile drawn from deck."));
        OnPlayerDrewTile.Broadcast();
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Tile drawn from discard pile."));
        OnPlayerDrewFromDiscard.Broadcast(DiscardDrawnTile);
    }
}

void AMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AMPlayerState, Flags, COND_OwnerOnly);
    DOREPLIFETIME(AMPlayerState, Seat);
    DOREPLIFETIME_CONDITION(AMPlayerState, PrivateHand, COND_OwnerOnly);
    DOREPLIFETIME(AMPlayerState, PublicTiles);
    DOREPLIFETIME_CONDITION(AMPlayerState, DrawnTile, COND_OwnerOnly);
    DOREPLIFETIME(AMPlayerState, DiscardDrawnTile);
}