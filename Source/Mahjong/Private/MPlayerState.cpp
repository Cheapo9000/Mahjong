/**
* @author Tim Lake
* @copyright 2026
*/

#include "MPlayerState.h"
#include "Net/UnrealNetwork.h"

void AMPlayerState::SetPlayerName(const FString& NewName)
{
    if (HasAuthority())
    {
        // We are the server
        Name = NewName;
        OnPlayerNameChanged.Broadcast(Name);
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
        // We are the server
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

        //OnPlayerHandChanged.Broadcast(PrivateHand);
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

        //OnPlayerHandChanged.Broadcast(PrivateHand);
    }
    else
    {
        // We are a client → ask server
        Server_Reset();
    }
}

void AMPlayerState::HandleNameChanged()
{
    // Update UI
}

void AMPlayerState::HandleSeatPositionChanged()
{
    // Update UI
}

void AMPlayerState::Server_SetPlayerName_Implementation(const FString& NewName)
{
    SetPlayerName(Name);
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

void AMPlayerState::OnRep_Name()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_Name called!"));
    // Called on clients when PlayerName updates
    OnPlayerNameChanged.Broadcast(Name);
}

void AMPlayerState::OnRep_SeatPosition()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_SeatPosition called!"));
    // Called on clients when PlayerName updates
    OnPlayerSeatPositionChanged.Broadcast(Seat);
}

void AMPlayerState::OnRep_Hand()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_Hand called!"));
    // Called on clients when PlayerName updates
    //OnPlayerHandChanged.Broadcast(PrivateHand);
}

void AMPlayerState::HandleHandChanged()
{
    // Update UI
}

void AMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMPlayerState, Name);
    DOREPLIFETIME(AMPlayerState, Seat);
    DOREPLIFETIME_CONDITION(AMPlayerState, PrivateHand, COND_OwnerOnly);
    DOREPLIFETIME(AMPlayerState, PublicTiles);
}