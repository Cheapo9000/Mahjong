#include "Table.h"
#include "CoreMinimal.h"
#include "Algo/Sort.h"
#include "Tile.h"
#include "MGameMode.h"
#include "MGameState.h"
#include "MPlayerController.h"
#include "MPlayerState.h"
#include "Net/UnrealNetwork.h"

UTable::UTable() {
	Deck = NewObject<UDeck>();
	Players.Reserve(MaxPlayers);
	CurrentPlayer = 0;
	PreviousPlayer = CurrentPlayer;
	PlayerCount = 0;
	FromDiscardPile = false;
}

int32 UTable::SeatPlayer(const FString& Name) {
	int32 Count = Players.Num();
	for (int i = 0; i < Count; ++i) {
		if (Players[i] == nullptr) {
			AMPlayerController* PC = (AMPlayerController*)GetWorld()->GetFirstPlayerController();
			if (!PC)
			{
				return -1;
			}
			AMPlayerState* PS = PC->GetPlayerState<AMPlayerState>();
			if (!PS)
			{
				return -1;
			}
			PS->SetPlayerSeatPosition(i);
			Players[i] = PC;
			++PlayerCount;

			return i;
		}
	}

	if (Count < MaxPlayers) {
		AMPlayerController* PC = (AMPlayerController*)GetWorld()->GetFirstPlayerController();
		if (!PC)
		{
			return -1;
		}
		AMPlayerState* PS = PC->GetPlayerState<AMPlayerState>();
		if (!PS)
		{
			return -1;
		}
		PS->SetPlayerSeatPosition(Players.Num());
		Players.Push(PC);
		++PlayerCount;

		return Count;
	}

	return -1;
}

void UTable::RemovePlayer(const int32& SeatPosition) {
	checkf(SeatPosition < 0 || SeatPosition >= Players.Num(), TEXT("RemovePlayer - Index out of range"));

	AMPlayerController* PC = Players[SeatPosition];
	if (!PC)
	{
		return;
	}
	AMPlayerState* PS = PC->GetPlayerState<AMPlayerState>();
	if (!PS)
	{
		return;
	}

	PS->SetPlayerSeatPosition(-1);
	Players[SeatPosition] = nullptr;
	--PlayerCount;
}

void UTable::DealHand(const int32& SeatPosition) {
	checkf(SeatPosition < 0 || SeatPosition >= Players.Num(), TEXT("DealHand - Index out of range"));

	if (!Deck)
	{
		return;
	}

	TArray<FTile> Hand = Deck->GetHand();
	Algo::Sort(Hand, [](const FTile& First, const FTile& Second)
		{
			if (First.Suit != Second.Suit)
			{
				return First.Suit < Second.Suit;
			}
			return First.Number < Second.Number;
		});
	AMGameMode* GM = GetWorld()->GetAuthGameMode<AMGameMode>();
	if (GM)
	{
		AMPlayerController* PC = Players[SeatPosition];
		if (PC) {
			AMPlayerState* PS = Cast<AMPlayerState>(PC->PlayerState);
			if (PS)
			{
				PS->SetPlayerHand(Hand);
			}
		}
	}
}

void UTable::DealHands() {
	for (int32 i = 0; i < Players.Num(); ++i) {
		DealHand(i);
	}
}

FTile UTable::DrawTile() {
	if (!Deck)
	{
		FTile Default;
		return Default;
	}

	DrawnTile = Deck->DrawTile();
	FromDiscardPile = false;

	return DrawnTile;
}

void UTable::DiscardDrawn() {
	AMGameState* GS = GetWorld()->GetGameState<AMGameState>();
	if (!GS)
	{
		return;
	}
	FTileArray* DiscardPile = &(GS->DiscardPile);
	if (DiscardPile) {
		Algo::Sort(DiscardPile->Items, [](const FTileEntry& First, const FTileEntry& Second)
			{
				if (First.Tile.Suit != Second.Tile.Suit)
				{
					return First.Tile.Suit < Second.Tile.Suit;
				}
				return First.Tile.Number < Second.Tile.Number;
			});
		DiscardPile->AddTile(DrawnTile);
	}
	DrawnTile.Suit = ESuit::None;
	DrawnTile.Number = 0;
	FromDiscardPile = false;
}

void UTable::KeepDrawn() {
	AMPlayerController* PC = Players[CurrentPlayer];
	if (!PC)
	{
		return;
	}
	AMPlayerState* PS = ((AMPlayerState*)PC->PlayerState);
	if (!PS)
	{
		return;
	}

	if (FromDiscardPile)
	{
		RevealSet(PS->PrivateHand, PS->PublicTiles);
		PS->PublicTiles.AddTile(DrawnTile);
		Algo::Sort(PS->PublicTiles.Items, [](const FTileEntry& First, const FTileEntry& Second)
			{
				if (First.Tile.Suit != Second.Tile.Suit)
				{
					return First.Tile.Suit < Second.Tile.Suit;
				}
				return First.Tile.Number < Second.Tile.Number;
			});
	}
	else
	{
		PS->PrivateHand.AddTile(DrawnTile);
	}
	Algo::Sort(PS->PrivateHand.Items, [](const FTileEntry& First, const FTileEntry& Second)
		{
			if (First.Tile.Suit != Second.Tile.Suit)
			{
				return First.Tile.Suit < Second.Tile.Suit;
			}
			return First.Tile.Number < Second.Tile.Number;
		});
	DrawnTile.Suit = ESuit::None;
	DrawnTile.Number = 0;
	FromDiscardPile = false;
}

bool UTable::DiscardTile(const int32& Id) {
	AMPlayerController* PC = Players[CurrentPlayer];
	if (!PC)
	{
		return false;
	}
	AMPlayerState* PS = ((AMPlayerState*)PC->PlayerState);
	if (!PS)
	{
		return false;
	}

	int32 Index = PS->PrivateHand.FindElemByID(Id);
	if (Index < 0) {
		return false;
	}

	AMGameState* GS = GetWorld()->GetGameState<AMGameState>();
	if (!GS)
	{
		return false;
	}
	FTileArray* DiscardPile = &(GS->DiscardPile);
	DiscardPile->AddTile(PS->PrivateHand.Items[Index].Tile);
	PS->PrivateHand.RemoveTile(Id);
	Algo::Sort(PS->PrivateHand.Items, [](const FTileEntry& First, const FTileEntry& Second)
		{
			if (First.Tile.Suit != Second.Tile.Suit)
			{
				return First.Tile.Suit < Second.Tile.Suit;
			}
			return First.Tile.Number < Second.Tile.Number;
		});

	return true;
}

void UTable::SetCurrentPlayer(const int32& SeatPosition, bool OverwritePrevious) {
	checkf(SeatPosition < 0 || SeatPosition >= Players.Num(), TEXT("SetCurrentPlayer - Index out of range"));

	for (int32 i = 0; i < Players.Num(); ++i)
	{
		if (Players[i] != nullptr)
		{
			AMPlayerState* PS = ((AMPlayerState*)Players[i]->PlayerState);
			if (PS)
			{
				PS->Flags &= ~EFlags::Peng;
				PS->Flags &= ~EFlags::Gong;
				PS->Flags &= ~EFlags::WantsPoG;
			}
		}
	}

	if (OverwritePrevious)
	{
		PreviousPlayer = SeatPosition - 1;
		// Prevent PreviousPlayer referencing a nullptr
		while (PreviousPlayer >= 0)
		{
			if (Players[PreviousPlayer] != nullptr)
			{
				break;
			}
		}
		// Prevent indexing out of the array
		if (PreviousPlayer < 0)
		{
			for (int32 i = Players.Num() - 1; i >= 0; --i) {
				if (Players[i] != nullptr) {
					PreviousPlayer = i;
					break;
				}
			}
		}
	}
	else
	{
		PreviousPlayer = CurrentPlayer;
	}

	CurrentPlayer = SeatPosition;
}

void UTable::NextPlayer() {
	for (int32 i = 0; i < Players.Num(); ++i)
	{
		if (Players[i] != nullptr)
		{
			AMPlayerState* PS = ((AMPlayerState*)Players[i]->PlayerState);
			if (PS)
			{
				PS->Flags &= ~EFlags::Peng;
				PS->Flags &= ~EFlags::Gong;
				PS->Flags &= ~EFlags::WantsPoG;
			}
		}
	}

	// Find the next player that is sat at the table.
	// This allows for gaps in the seating arrangement and allows for
	// two players to sit across the table.
	// This code assumes that if you have a name then you have a seat as well.
	PreviousPlayer = CurrentPlayer;
	for (int32 i = 0; i < Players.Num(); ++i) {
		// This number is in base 1.
		CurrentPlayer = (CurrentPlayer + 1) % Players.Num();
		if (Players[CurrentPlayer] != nullptr) {
			return;
		}
	}
}

void UTable::SetWantsPeng(const int32 Seat) {
	checkf(Seat < 0 || Seat >= Players.Num(), TEXT("SetWantsPeng - Index out of range"));

	if (Players[Seat]) {
		AMPlayerState* PS = ((AMPlayerState*)Players[Seat]->PlayerState);
		if (PS)
		{
			PS->Flags |= EFlags::Peng;
			PS->Flags |= EFlags::WantsPoG;
		}
	}
}

void UTable::WelcomePLayers() const {
	// TODO: Display Welcome
}

void UTable::DisplayTable(const int32& SeatPosition) const {
	// Display Table
}

bool UTable::CanChi() const {
	AMGameState* GS = GetWorld()->GetGameState<AMGameState>();
	if (!Players[CurrentPlayer]) {
		return false;
	}
	AMPlayerState* PS = ((AMPlayerState*)Players[CurrentPlayer]->PlayerState);

	if (!GS || !PS) {
		return false;
	}

	if (GS->DiscardPile.Num() <= 0) {
		return false;
	}

	FTile LastTile = GS->DiscardPile[GS->DiscardPile.Num() - 1];
	if (LastTile.Suit == ESuit::Wind || LastTile.Suit == ESuit::Dragon) {
		return false;
	}

	// Make the copy of the player hand only contain unique tiles
	// Exclude LastTile if it was in the Player's hand
	TArray<FTile> PrivateHand = PS->PrivateHand.GetTiles();
	TArray<FTile> Hand;
	for (int32 i = 0; i < PrivateHand.Num(); ++i) {
		if (PrivateHand[i] != LastTile) {
			Hand.AddUnique(PrivateHand[i]);
		}
	}
	Algo::Sort(Hand, [](const FTile& First, const FTile& Second)
		{
			if (First.Suit != Second.Suit)
			{
				return First.Suit < Second.Suit;
			}
			return First.Number < Second.Number;
		});

	FTile CurrentTile;
	for (int32 i = 0; i < Hand.Num(); ++i) {
		CurrentTile = Hand[i];
		if (CurrentTile.Suit == LastTile.Suit) {
			if (LastTile.Number + 1 == CurrentTile.Number) {
				if (i + 1 >= Hand.Num()) {
					return false;
				}
				// Check if the LastTile is the start of a straight
				else if ((CurrentTile.Suit == Hand[i + 1].Suit)
					&& (CurrentTile.Number + 1 == Hand[i + 1].Number)) {
					return true;
				}
				else {
					return false;
				}
			}
			else if (LastTile.Number == CurrentTile.Number + 1) {
				if ((i + 1 >= Hand.Num()) || (i - 1 < 0)) {
					return false;
				}
				// Check if the LastTile is the end of a straght
				else if ((CurrentTile.Suit == Hand[i - 1].Suit)
					&& (CurrentTile.Number - 1 == Hand[i - 1].Number)) {
					return true;
				}
				// Check if the LastTle is the middle of a staight
				else if ((LastTile.Suit == Hand[i + 1].Suit)
					&& (LastTile.Number + 1 == Hand[i + 1].Number)) {
					return true;
				}
				else {
					return false;
				}
			}
		}
	}

	return false;
}

void UTable::Chi() {
	AMGameState* GS = GetWorld()->GetGameState<AMGameState>();

	if (!CanChi()) {
		return;
	}

	DrawnTile = GS->DiscardPile[GS->DiscardPile.Num() - 1];
	FromDiscardPile = true;
	GS->DiscardPile.RemoveTile(DrawnTile.Id);
}

EFlags UTable::CanPengOrGong(const int32& SeatPosition) const {
	AMGameState* GS = GetWorld()->GetGameState<AMGameState>();
	if (!Players[CurrentPlayer]) {
		return EFlags::None;
	}
	AMPlayerState* PS = ((AMPlayerState*)Players[CurrentPlayer]->PlayerState);

	if (!GS || !PS) {
		return EFlags::None;
	}

	if (GS->DiscardPile.Num() <= 0) {
		return EFlags::None;
	}

	checkf(SeatPosition < 0 || SeatPosition >= Players.Num(), TEXT("CanPengOrGong - Index out of range"));

	// Previous player or players not seated can't Peng or Gong
	if (SeatPosition > PlayerCount || PreviousPlayer == SeatPosition) {
		return EFlags::None;
	}

	TArray<FTile> Hand = PS->PrivateHand.GetTiles();
	FTile LastTile = GS->DiscardPile[GS->DiscardPile.Num() - 1];
	FTile CurrentTile;
	FTile NextTile;
	FTile NextNextTile;
	for (int32 i = 0; i < Hand.Num(); ++i) {
		CurrentTile = Hand[i];
		if (CurrentTile.Suit == LastTile.Suit) {
			if (CurrentTile.Number == LastTile.Number) {
				if (i + 1 < Hand.Num()) {
					NextTile = Hand[i + 1];
					if (CurrentTile.Suit == NextTile.Suit) {
						if (CurrentTile.Number == NextTile.Number) {
							// There's at least a pair so we can Peng
							if (i + 2 < Hand.Num()) {
								NextNextTile = Hand[i + 2];
								if (CurrentTile.Suit == NextNextTile.Suit) {
									if (CurrentTile.Number == NextNextTile.Number) {
										PS->Flags |= EFlags::Gong;
										PS->Flags |= EFlags::WantsPoG;
										return EFlags::Gong;
									}
								}
								else {
									PS->Flags |= EFlags::Peng;
									PS->Flags |= EFlags::WantsPoG;
									return EFlags::Peng;
								}
							}
							else {
								PS->Flags |= EFlags::Peng;
								PS->Flags |= EFlags::WantsPoG;
								return EFlags::Peng;
							}
						}
						else {
							PS->Flags &= ~EFlags::Peng;
							PS->Flags &= ~EFlags::Gong;
							PS->Flags &= ~EFlags::WantsPoG;
							return EFlags::None;
						}
					}
					else {
						PS->Flags &= ~EFlags::Peng;
						PS->Flags &= ~EFlags::Gong;
						PS->Flags &= ~EFlags::WantsPoG;
						return EFlags::None;
					}
				}
				else {
					PS->Flags &= ~EFlags::Peng;
					PS->Flags &= ~EFlags::Gong;
					PS->Flags &= ~EFlags::WantsPoG;
					return EFlags::None;
				}
			}
		}
	}

	PS->Flags &= ~EFlags::Peng;
	PS->Flags &= ~EFlags::Gong;
	PS->Flags &= ~EFlags::WantsPoG;
	return EFlags::None;
}

EFlags UTable::CanPengOrGongAll() const {
	EFlags Results;
	EFlags Highest = EFlags::None;

	AMGameState* GS = GetWorld()->GetGameState<AMGameState>();

	if (!GS) {
		return EFlags::None;
	}

	if (GS->DiscardPile.Num() <= 0) {
		return EFlags::None;
	}

	for (int32 i = 0; i < Players.Num(); ++i) {
		Results = CanPengOrGong(i);
		if (Results > Highest) {
			Highest = Results;
		}
		if (Highest >= EFlags::Gong) {
			break;
		}
	}

	return Highest;
}

EFlags UTable::PengOrGong(const int32& SeatPosition) {
	checkf(SeatPosition < 0 || SeatPosition >= Players.Num(), TEXT("PengOrGong - Index out of range"));

	AMGameState* GS = GetWorld()->GetGameState<AMGameState>();

	if (!GS) {
		return EFlags::None;
	}

	EFlags PengOrGong = CanPengOrGong(SeatPosition);
	if (PengOrGong != EFlags::Peng && PengOrGong != EFlags::Gong) {
		return PengOrGong;
	}

	DrawnTile = GS->DiscardPile[GS->DiscardPile.Num() - 1];
	FromDiscardPile = true;
	GS->DiscardPile.RemoveTile(DrawnTile.Id);

	return PengOrGong;
}

void UTable::DisplayPengOrGongPlayers() const {
	// TODO: Display Peng Or Gong Players
}

void UTable::RevealSet(FTileArray& Hand, FTileArray& PublicHand) {
	// 0 = PreviousTile, 1 = NextTile, 2 = NextNextTile
	int32 TileIndices[3] = { 0, 0, 0 };
	FTile PreviousTile;
	FTile CurrentTile;
	FTile NextTile;
	FTile NextNextTile;
	for (int32 i = 0; i < Hand.Num(); ++i) {
		CurrentTile = Hand[i];
		if (CurrentTile.Suit == DrawnTile.Suit) {
			if (DrawnTile.Number + 1 == CurrentTile.Number) {
				if (i + 1 >= Hand.Num()) {
					return;
				}

				for (int32 j = i; j + 1 < Hand.Num(); ++j) {
					NextTile = Hand[j + 1];
					TileIndices[1] = j + 1;

					if (CurrentTile != NextTile) {
						break;
					}
				}
				// Check if the DrawnTile is the start of a straight
				if ((CurrentTile.Suit == NextTile.Suit)
					&& (CurrentTile.Number + 1 == NextTile.Number)) {
					PublicHand.AddTile(Hand[i]);
					PublicHand.AddTile(Hand[TileIndices[1]]);
					Hand.RemoveTile(Hand[i].Id);
					Hand.RemoveTile(Hand[TileIndices[1]].Id);
					return;
				}
				else {
					return;
				}
			}
			else if (DrawnTile.Number == CurrentTile.Number + 1) {
				bool AtStart = i - 1 < 0;
				bool AtEnd = i + 1 >= Hand.Num();
				
				for (int32 j = i; j - 1 >= 0; --j) {
					PreviousTile = Hand[j - 1];
					TileIndices[0] = j - 1;

					if (CurrentTile != PreviousTile) {
						break;
					}
				}
				for (int32 j = i; j + 1 < Hand.Num(); ++j) {
					NextNextTile = Hand[j + 1];
					TileIndices[2] = j + 1;

					if (CurrentTile != NextNextTile) {
						break;
					}
				}
				// Check if the DrawnTile is the end of a straght
				if ((CurrentTile.Suit == PreviousTile.Suit && !AtStart)
					&& (CurrentTile.Number - 1 == PreviousTile.Number)) {
					PublicHand.AddTile(Hand[i]);
					PublicHand.AddTile(Hand[TileIndices[0]]);
					Hand.RemoveTile(Hand[i].Id);
					Hand.RemoveTile(Hand[TileIndices[0]].Id);
					return;
				}
				// Check if the DrawnTile is the middle of a staight
				else if ((DrawnTile.Suit == NextNextTile.Suit && !AtEnd)
					&& (DrawnTile.Number + 1 == NextNextTile.Number)) {
					PublicHand.AddTile(Hand[i]);
					PublicHand.AddTile(Hand[TileIndices[2]]);
					Hand.RemoveTile(Hand[i].Id);
					Hand.RemoveTile(Hand[TileIndices[2]].Id);
					return;
				}
				else {
					return;
				}
			}
		}
	}
}

bool UTable::CheckWin() const {
	// There are two win conditions.
	// First win condition: 4 sets of 3 and a pair
	// Second win condition: 1 set of 4, 2 sets of 3, and 2 pairs

	if (!Players[CurrentPlayer]) {
		return false;
	}
	AMPlayerState* PS = ((AMPlayerState*)Players[CurrentPlayer]->PlayerState);
	if (!PS) {
		return false;
	}
	TArray<FTile> Hand = PS->PrivateHand.GetTiles();
	Hand.Append(PS->PublicTiles.GetTiles());
	// Temporarily include the drawn tile to see if this tile makes a winning hand
	Hand.Add(DrawnTile);
	// Sorting makes it easier to identify a winning hand
	Algo::Sort(Hand, [](const FTile& First, const FTile& Second)
		{
			if (First.Suit != Second.Suit)
			{
				return First.Suit < Second.Suit;
			}
			return First.Number < Second.Number;
		});

	FTile PrevTile;
	FTile CurTile;
	int32 Pair = 0;
	int32 Triple = 0;
	int32 Quad = 0;
	bool BMatch = false; //Is this building a matching set
	int32 SetCount = 1; //The first tile is starting a set
	// start at 1 because 0 is the start of the set
	for (int32 i = 1; i < Hand.Num(); ++i) {
		PrevTile = Hand[i - 1];
		CurTile = Hand[i];
		if (PrevTile.Suit == CurTile.Suit) {
			if (PrevTile.Number + 1 == CurTile.Number) {
				++SetCount;
				if (SetCount == 3) {
					++Triple;
					SetCount = 1;
					++i;
				}
			}
			else if (PrevTile.Number == CurTile.Number) {
				if (SetCount > 1 && !BMatch) {
					if (SetCount == 2) {
						++Pair;
						SetCount = 0;
						if (Pair > 2) {
							return false;
						}
					}
					else if (SetCount == 3) {
						++Triple;
						SetCount = 0;
					}
					else if (SetCount == 4) {
						++Quad;
						SetCount = 0;
					}
				}
				++SetCount;
				if (SetCount == 3 && BMatch) {
					// Check if we have triple or quad & mark accordingly
					// Check if this is the last tile in the hand
					if (i + 1 >= Hand.Num()) {
						++Triple;
						SetCount = 0;
						BMatch = false;
						break; // This is the last tile so exit the loop
					}
					else { // There is another next tile so check for quad
						FTile NextTile = Hand[i + 1];
						if ((CurTile.Suit == NextTile.Suit)
							&& (CurTile.Number == NextTile.Number)) {
							++Quad;
							SetCount = 1;
							BMatch = false;
							i += 2; // This can go beyond the hand size, but the loop check will prevent out of bounds indices
							continue;
						}
						else {
							// Check for edge case to determine if it's a double next to a straight or a triple next to a straight
							if ((CurTile.Suit == NextTile.Suit)
								&& (CurTile.Number + 1 == NextTile.Number)) {
								int32 TempTriple = 0;
								int32 TempSetCount = 2;
								FTile TempPrevTile;
								FTile TempCurTile;
								for (int32 j = i + 2; j < Hand.Num(); ++j) {
									TempPrevTile = Hand[j - 1];
									TempCurTile = Hand[j];
									if ((TempPrevTile.Suit == TempCurTile.Suit)
										&& (TempPrevTile.Number + 1 == TempCurTile.Number)) {
										++TempSetCount;
										if (TempSetCount == 3) {
											++TempTriple;
											TempSetCount = 0;
										}
									}
									else {
										if (TempSetCount == 0 && TempTriple > 0) {
											Triple += TempTriple;
											SetCount = 1;
											++Pair;
											BMatch = false;
											i = j;
											break;
										}
										else if (TempSetCount == 1 && TempTriple > 0) {
											// Starting assumption was a pair and a straight,
											// but actually was two triples
											Triple += TempTriple + 1;
											SetCount = 0;
											BMatch = false;
											i = j - 1;
											++i;
											break;
										}
										else {
											++Pair;
											Triple += TempTriple;
											SetCount = 1;
											BMatch = false;
											i = j - TempSetCount;
											break;
										}
									}
								}
							}
							else {
								++Triple;
								SetCount = 1;
								BMatch = false;
								++i;
							}
							continue;
						}
					}
				}
				else if (SetCount == 2 && !BMatch) {
					if (i + 1 >= Hand.Num()) {
						++Pair;
						SetCount = 1;
						if (Pair > 2) {
							return false;
						}
						++i;
						break; // This is the last tile so exit the loop
					}
					else { // There is another next tile so check for triple
						FTile NextTile = Hand[i + 1];
						// Check for edge case to determine if it's a double next to a straight or a triple next to a straight
						if ((CurTile.Suit == NextTile.Suit)
							&& (CurTile.Number + 1 == NextTile.Number)) {
							++Pair;
							BMatch = false;
							SetCount = 1;
							if (Pair > 2) {
								return false;
							}
							++i;
							continue;
						}
						else if ((CurTile.Suit == NextTile.Suit)
							&& (CurTile.Number == NextTile.Number)) {
							int32 TempTriple = 0;
							int32 TempSetCount = 1;
							FTile TempPrevTile;
							FTile TempCurTile;
							for (int32 j = i + 2; j < Hand.Num(); ++j) {
								TempPrevTile = Hand[j - 1];
								TempCurTile = Hand[j];
								if ((TempPrevTile.Suit == TempCurTile.Suit)
									&& (TempPrevTile.Number + 1 == TempCurTile.Number)) {
									++TempSetCount;
									if (TempSetCount == 3) {
										++TempTriple;
										TempSetCount = 0;
									}
								}
								if ((TempPrevTile.Suit == TempCurTile.Suit)
									&& (TempPrevTile.Number == TempCurTile.Number)) {
									// Could be a quad or a triple with a straight
									int32 TempTriple2 = 0;
									int32 TempSetCount2 = 1;
									FTile TempPrevTile2;
									FTile TempCurTile2;
									for (int32 k = j + 1; k < Hand.Num(); ++k) {
										TempPrevTile2 = Hand[k - 1];
										TempCurTile2 = Hand[k];
										if ((TempPrevTile.Suit == TempCurTile.Suit)
											&& (TempPrevTile.Number + 1 == TempCurTile.Number)) {
											// Check if triple and straights or quad and straights
											++TempSetCount2;
											if (TempSetCount2 == 3) {
												++TempTriple2;
												TempSetCount2 = 0;
											}
										}
										else {
											if (TempSetCount2 == 2 && TempTriple2 > 0) {
												++Quad;
												Triple += TempTriple + TempTriple2;
												SetCount = 1;
												BMatch = false;
												i = k - 1;
												break;
											}
											else {
												++Quad;
												Triple += TempTriple + TempTriple2;
												SetCount = 1;
												BMatch = false;
												i = k;
												break;
											}
										}
									}
								}
								else {
									if (TempSetCount == 0 && TempTriple > 0) {
										Triple += TempTriple;
										SetCount = 1;
										++Pair;
										BMatch = false;
										i = j;
										break;
									}
									else if (TempSetCount == 1 && TempTriple > 0) {
										// Starting assumption was a pair and a straight,
										// but actually was two triples
										Triple += TempTriple + 1;
										SetCount = 0;
										BMatch = false;
										i = j - 1;
										++i;
										break;
									}
									else {
										++Triple;
										Triple += TempTriple;
										SetCount = 1;
										BMatch = false;
										i = j - TempSetCount;
										++i;
										break;
									}
								}
							}
							if (i + 2 < Hand.Num()) {
								// If the for loop was entered then don't update match to true
								continue;
							}
						}
					}
				}
				BMatch = true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	if (Pair == 1 && (Triple == 4 && Quad == 0)) {
		return true;
	}
	else if (Pair == 2 && (Triple == 2 && Quad == 1)) {
		return true;
	}

	return false;
}

FTile UTable::GetDrawnTile() const {
	return DrawnTile;
}

AMPlayerController* UTable::GetPlayer(const int32& SeatPosition) {
	checkf(SeatPosition < 0 || SeatPosition >= Players.Num(), TEXT("GetPlayer - Index out of range"));

	return Players[SeatPosition];
}

TArray<FTile> UTable::GetPlayerHand(const int32& SeatPosition) {
	checkf(SeatPosition < 0 || SeatPosition >= Players.Num(), TEXT("GetPlayerHand - Index out of range"));

	if (!Players[SeatPosition])
	{
		TArray<FTile> Empty;
		return Empty;
	}
	AMPlayerState* PS = ((AMPlayerState*)Players[SeatPosition]->PlayerState);
	if (!PS)
	{
		TArray<FTile> Empty;
		return Empty;
	}

	return PS->PrivateHand.GetTiles();
}

UDeck* UTable::GetDeck() {
	return Deck;
}

int32 UTable::GetCurrentPlayer() const {
	return CurrentPlayer;
}