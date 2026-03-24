/**
 * @file player.h
 * @brief Represents a Mahjong player with name, seat position, and hand of tiles.
 *
 * Provides methods for setting and getting each value.
 */
#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "tile.h"
using namespace std;

/**
 * @class player
 * @brief Represents a single player with a name, seat position, and hand.
 *
 * Provides methods for setting and getting each value.
 */
class player {
	string name;
	int position;
	vector<tile> hand;
	bool hasShown;

public:
	/**
	 * @brief Sets the initial values of the player to a blank name, a negative seat position, and an empty hand.
	 */
	player();

	/**
	 * @brief Sets the name of the player to the given name.
	 */
	void setName(string name);

	/**
	 * @brief Sets the seat position of the player to the given position.
	 */
	void setSeatPos(int position);

	/**
	 * @brief Sets the hand of the player to the given hand tiles.
	 */
	void setHand(vector<tile>& hand);

	/**
	 * @brief Sets the state determining if any tiles are visible.
	 */
	void setShownTiles(bool shown);

	/**
	 * @brief Sets the initial values of the player to a blank name, a negative seat position, and an empty hand.
	 */
	void reset();

	/**
	 * @brief Gets the name of the player.
	 * @return A string that contains the name of the player.
	 * An empty string means that the player has not been given values.
	 */
	string getName() const;

	/**
	 * @brief Gets the seat position of the player.
	 * @return An integer that contains the seat position of the player.
	 * A negative number means that the player has not been given values.
	 */
	int getSeatPos() const;

	/**
	 * @brief Gets the hand size of the player.
	 * @return An integer that contains the size of the hand of the player.
	 */
	int getHandSize() const;

	/**
	 * @brief Gets the hand of the player
	 * @return A vector of tiles that contains the hand of the player.
	 * This method is const safe.
	 */
	vector<tile> c_getHand() const;

	/**
	 * @brief Gets a pointer to the hand of the player.
	 * @return A pointer to a vector of tiles that contains the hand of the player.
	 */
	vector<tile>* getHand();

	/**
	 * @brief Checks if the player has any shown tiles
	 * @return A bool contianing true if there are any tiles shown.
	 */
	bool hasShownTiles();
};

#endif