/**
 * @file deck.h
 * @brief Represents a Mahjong deck with tiles, maximum tiles in the deck, and the size of a starting hand.
 *
 * Provides methods for setting and getting each value as well as drawing a single tile at random.
 */
#ifndef DECK_H
#define DECK_H

#include <vector>
#include <random>
#include "tile.h"
#include "player.h"
using namespace std;

/**
 * @class deck
 * @brief Represents a Mahjong deck with tiles, maximum tiles in the deck, and the size of a starting hand.
 *
 * Provides methods for setting and getting each value as well as drawing a single tile at random.
 */
class deck {

	const int maxTiles = 136;
	const int handSize = 13;
	vector<tile> tiles;

	/**
	 * @brief Sets the initial values of the tiles to a standard Mahjong set without the 8 bonus tiles.
	 * @details https://en.wikipedia.org/wiki/Mahjong_tiles#:~:text=A%20set%20of%20Mahjong%20tiles,replace%20damaged%20or%20missing%20tiles.
	 */
	void build();

public:
	/**
	 * @brief Sets the initial values of the tiles to a standard Mahjong set without the 8 bonus tiles.
	 */
	deck();

	/**
	 * @brief Removes the tiles from all the players and the discard pile and returns them to the deck. If a null pointer is given to a parameter then no tiles are taken from the vector.
	 */
	void reset(vector<player>* players, vector<tile>* discarded);

	/**
	 * @brief Adds the vector of tiles to the end of the deck.
	 */
	void addTiles(vector<tile>& tiles);

	/**
	 * @brief Replaces the tiles in the deck with the given tiles.
	 */
	void setTiles(vector<tile>& tiles);

	/**
	 * @brief Gets the size of the starting hand of the player.
	 */
	int getHandSize() const;

	/**
	 * @brief Gets all the tiles of the deck.
	 * @return A pointer to a vector of tiles that represent the tiles in the deck.
	 */
	vector<tile>* getTiles();

	/**
	 * @brief Gets a hand of tiles for a player determined by the maximum starting hand size.
	 * @return A vector of tiles that contains the tiles randomly drawn from the deck up to the amount specified by the maximum starting hand size. These drawn tiles are removed from the deck.
	 * @throw runtime_error can be thrown if a tile's value cannot be set.
	 */
	vector<tile> getHand();

	/**
	 * @brief Gets a single random tile from the deck.
	 * @return A tile that is randomly removed from the deck.
	 */
	tile drawTile();

};

#endif // DECK_H