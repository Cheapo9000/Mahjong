/**
 * @file table.h
 * @author Tim Lake
 * @copyright 2026
 * @brief Represents a Mahjong table with players, a discard pile, a deck of tiles, a drawn tile, and the current player.
 *
 * Provides methods for setting, getting, and displaying each value.
 */
#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <algorithm>
#include "player.h"
#include "deck.h"
using namespace std;

/**
 * @class table
 * @brief Represents a Mahjong table with players, a discard pile, a deck of tiles, a drawn tile, and the current player.
 *
 * Provides methods for setting, getting, and displaying each value.
 */
class table {
	const int maxPlayers = 4;
	vector<player> players;
	vector<tile> discardPile;
	deck walls;
	tile drawnTile;
	int previousPlayer;
	int currentPlayer;
	int playerCount;
	mutable vector<pair<int, bool>> wantsPengOrGong;

public:
	/**
	 * @brief Sets the initial values of the table with number of players equal to maxPlayers, an empty discard pile, and the current player at zero.
	 */
	table();

	/**
	 * @brief Sets a player at the next empty seat with the given name.
	 * @return An integer that contains the seat number of the player or a negative one if there were no more seats available.
	 */
	int seatPlayer(string name);

	/**
	 * @brief Removes the plyaer at the given seat position.
	 * @throw out_of_range exception if the seat posiiton is above or below the indices of available seats specified by maxPlayers.
	 */
	void removePlayer(int seatPosition);

	/**
	 * @brief Deals a sorted hand from the deck to the player in the specified seat position.
	 * @throw out_of_range exception if the seat posiiton is above or below the indices of available seats specified by maxPlayers.
	 */
	void dealHand(int seatPosition);

	/**
	 * @brief Deals a sorted hand to each seated player from the deck.
	 * @throw out_of_range exception can be thrown if the seat posiiton is above or below the indices of available seats specified by maxPlayers.
	 */
	void dealHands();

	/**
	 * @brief Gets a tile a random from the deck.
	 * @return A tile that was removed from the deck.
	 */
	tile drawTile();

	/**
	 * @brief Adds the drawn tile to the discard pile and clears the value of the drawn tile.
	 */
	void discardDrawn();

	/**
	 * @brief Adds the drawn tile to the sorted hand of the current player and clears the value of the drawn tile.
	 */
	void keepDrawn();

	/**
	 * @brief Removes the tile at the given index from the sorted hand of the current player and adds it onto a sorted discard pile. The last tile is not sorted since it may be taken by other players.
	 * @throw out_of_range exception if the index is above or below the indices of tiles in the player's hand.
	 * @return A bool if the tile was successfully discarded.
	 */
	bool discardTile(int index);

	/**
	 * @brief Sets the current player. If overwritePrev is set to true then the previous player will be marked as the one that came in order before the provided seat regardless of what the actual previous player was. Also resets wantsPengOrGong values to zeroes and false.
	 * @throw out_of_range exception if the seat is above or below the indices of players.
	 */
	void setCurrentPlayer(int seat, bool overwritePrev = false);

	/**
	 * @brief Sets the current player to the next player.  Also resets wantsPengOrGong values to zeroes and false.
	 */
	void nextPlayer();

	/**
	 * @brief Sets the player in the given seat as wants to Peng
	 * @throw out_of_range exception if the seat is above or below the indices of players.
	 */
	void setWantsPeng(int seat);

	/**
	 * @brief Displays a welcome message addressing each player with a name.
	 */
	void welcomePlayers() const;

	/**
	 * @brief Shows the table from the point of view of the provided seat position. A negative one will show the table from the perspective of someone without a hand.
	 * @throw out_of_range exception if the seat is above or below the indices of players.
	 */
	void displayTable(int seatPosition = -1) const;

	/**
	 * @brief Shows the discard pile contents.
	 */
	void displayDiscardPile() const;

	/**
	 * @brief Checks if the current player can Chi the tile in the discard pile.
	 * @return A bool value containing true if the player can retrieve the tile from the discard pile.
	 * @throw runtime_error if a tile number and/or suit cannot be set
	 */
	bool canChi() const;

	/**
	 * @brief Places the last tile in the discard pile into the drawn tile
	 */
	void chi();

	/**
	 * @brief Checks if the player in seatPosition can Peng or Gong the tile in the discard pile.
	 * @return An integer value containing 0 if the player can Peng or Gong, 1 if the player can Peng, or 2 if the player can Gong the tile from the discard pile.
	 * @throw out_of_range can occur if seatPosition is beyond player indices.
	 */
	int canPengOrGong(int seatPosition) const;

	/**
	 * @brief Checks if any player except the previous player can Peng or Gong the tile in the discard pile.
	 * @return An integer value containing 0 if nobody can Peng or Gong, 1 if at least one player can Peng, or 2 if at least one player can Gong the tile from the discard pile.
	 * @throw out_of_range can occur if seatPosition is beyond player indices.
	 */
	int canPengOrGong() const;

	/**
	 * @brief Attempts to peng or gong for the player at the given seat position.
	 * @return An integer value containing 0 if the player couldn't peng or gong, 1 if the player was able to Peng, or 2 if the player was able to Gong the tile from the discard pile.
	 * @throw out_of_range can occur if seatPosition is beyond player indices.
	 */
	int pengOrGong(int seatPosition);

	/**
	 * @brief Shows the players that can currently peng or gong and their seat position
	 */
	void displayPengOrGongPlayers() const;

	/**
	 * @brief Reveals the other two tiles in the consecutive set that the drawn tile completes.
	 */
	void revealSet(vector<tile>* hand);

	/**
	 * @brief Checks if the current player is at a win state considering their hand and the drawn tile.
	 * @return A bool value containing true if the player hand and the drawn tile equals a winning hand.
	 */
	bool checkWin() const;

	/**
	 * @brief Gets the drawn tile.
	 * @return A tile that contains the drawn tile information.
	 */
	tile getDrawnTile() const;

	/**
	 * @brief Gets the player at the given seat position.
	 * @return A pointer to the player at the index of seatPosition.
	 * @throw out_of_range exception if the seat position is above or below the indices of players.
	 */
	player* getPlayer(int seatPosition);

	/**
	 * @brief Gets the player's hand at the given seat position.
	 * @return A pointer to the vector of tiles of the player's hand at the index of seatPosition.
	 * @throw out_of_range exception if the seat position is above or below the indices of players.
	 */
	vector<tile>* getPlayerHand(int seatPosition);

	/**
	 * @brief Checks if the discard pile has tiles in it.
	 * @return A bool containing true if there are tiles n the discard pile, otherwise false.
	 */
	bool hasDiscardedTiles() const;

	/**
	 * @brief Gets the deck.
	 * @return A pointer to the deck.
	 */
	deck* getDeck();

	/**
	 * @brief Gets the current player.
	 * @return An integer containing the seat position of the current player.
	 */
	int getCurrentPlayer() const;
};

#endif // TABLE_H
