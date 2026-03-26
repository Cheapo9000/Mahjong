#include "table.h"
#include <iostream>
#include <algorithm>

table::table() {
	players = vector<player>(maxPlayers);
	discardPile = vector<tile>();
	currentPlayer = 0;
}

int table::seatPlayer(string name) {
	for (int i = 0; i < players.size(); ++i) {
		if (players.at(i).getName().empty()) {
			players.at(i).setName(name);
			return i;
		}
	}

	return -1;
}

void table::removePlayer(int seatPosition) {
	if (seatPosition < 0 || seatPosition >= players.size()) {
		throw out_of_range("Index out of range");
	}

	players.at(seatPosition).reset();
}

void table::dealHand(int seatPosition) {
	if (seatPosition < 0 || seatPosition >= players.size()) {
		throw out_of_range("Index out of range");
	}

	vector<tile> hand;
	try {
		hand = walls.getHand();
	}
	catch (...) {
		throw runtime_error("Could not set the number or suit on a tile.");
	}
	sort(hand.begin(), hand.end());
	players.at(seatPosition).setHand(hand);
}

void table::dealHands() {
	for (int i = 0; i < players.size(); ++i) {
		try {
			dealHand(i);
		}
		catch (...) {
			throw runtime_error("Couldn't deal hands due to invalid index");
		}
	}
}

tile table::drawTile() {
	drawnTile = walls.drawTile();

	return drawnTile;
}

void table::discardDrawn() {
	sort(discardPile.begin(), discardPile.end());
	discardPile.push_back(drawnTile);
	drawnTile.reset();
}

void table::keepDrawn() {
	vector<tile>* hand = players.at(currentPlayer).getHand();
	// Reveal tiles making the set that came from the discard pile
	if (drawnTile.isShown()) {
		players.at(currentPlayer).setShownTiles(true);
		revealSet(hand);
	}
	hand->push_back(drawnTile);
	sort(hand->begin(), hand->end());
	drawnTile.reset();
}

bool table::discardTile(int index) {
	vector<tile>* hand = players.at(currentPlayer).getHand();
	if (index < 0 || index >= hand->size()) {
		throw out_of_range("Index out of range");
	}

	if (hand->at(index).isShown()) {
		return false;
	}

	discardPile.push_back(hand->at(index));
	try {
		hand->at(index).setNumber(hand->back().getNumber());
		hand->at(index).setSuit(hand->back().getSuit());
		hand->at(index).setShown(hand->back().isShown());
	}
	catch (...) {
		throw runtime_error("Could not set the number or suit on a tile.");
	}
	hand->pop_back();
	sort(hand->begin(), hand->end());

	return true;
}

void table::setCurrentPlayer(int seat) {
	if (seat < 0 || seat >= players.size()) {
		throw out_of_range("Index out of range");
	}

	currentPlayer = seat;
}

void table::nextPlayer() {
	// Find the next player that is sat at the table.
	// This allows for gaps in the seating arrangement and allows for
	// two players to sit across the table.
	// This code assumes that if you have a name then you have a seat as well.
	for (int i = 0; i < players.size(); ++i) {
		// This number is in base 1.
		currentPlayer = (currentPlayer + 1) % players.size();
		if (!players.at(currentPlayer).getName().empty()) {
			return;
		}
	}
}

void table::welcomePlayers() const {
	int count = 0;
	string name = "";
	cout << "Welcome ";
	for (int i = 0; i < players.size() - 1; ++i) {
		name = players.at(i).getName();
		if (!name.empty()) {
			++count;
		}
	}
	for (int i = 0; i < count - 1; ++i) {
		name = players.at(i).getName();
		if (!name.empty()) {
			if (i > 0) {
				cout << ", ";
			}
			cout << name;
		}
	}
	if (count > 1) {
		if (count > 2) {
			cout << ",";
		}
		cout << " and ";
	}
	cout << players.at(count - 1).getName() << "!" << endl;
}

void table::displayTable(int seatPosition) const {
	for (int i = 0; i < players.size(); ++i) {
		if (i == seatPosition) {
			continue;
		}
		if (players.at(i).hasShownTiles()) {
			cout << players.at(i).getName() << "'s shown tiles:" << endl;
			vector<tile> otherHand = players.at(i).c_getHand();
			for (int j = 0; j < otherHand.size(); ++j) {
				if (otherHand.at(j).isShown()) {
					cout << "|" << otherHand.at(j).getDisplayValue();
				}
			}
			cout << "|" << endl;
		}
	}

	player player = players.at(seatPosition);
	vector<tile> hand = player.c_getHand();
	cout << endl << "Your tiles:" << endl;
	for (int i = 0; i < hand.size(); ++i) {
		if (i < 9) {
			cout << "| " << i + 1;
		}
		else {
			cout << "|" << i + 1;
		}
	}
	cout << "|" << endl;
	for (int i = 0; i < hand.size(); ++i) {
		cout << "|" << hand.at(i).getDisplayValue();
	}
	cout << "|" << endl;
	for (int i = 0; i < hand.size(); ++i) {
		if (hand.at(i).isShown()) {
			cout << "|**";
		}
		else {
			cout << "|  ";
		}
	}
	cout << "|" << endl << endl;
}

void table::displayDiscardPile() const {
	for (int i = 0; i < discardPile.size(); ++i) {
		if ((i % 20) == 0 && i > 0) {
			cout << "|" << endl;
		}
		cout << "|" << discardPile.at(i).getDisplayValue();
	}
	cout << "|" << endl << endl;
}

bool table::canChi() const {
	if (discardPile.size() < 1) {
		return false;
	}

	tile lastTile = discardPile.at(discardPile.size() - 1);
	if (lastTile.getSuit() == wind || lastTile.getSuit() == dragon) {
		return false;
	}

	// Make the copy of the player hand only contain unique tiles
	vector<tile> hand = players.at(currentPlayer).c_getHand();
	vector<tile>::iterator iter = unique(hand.begin(), hand.end());
	hand.erase(iter, hand.end());

	// If the lastTile is in the player hand. Remove it.
	iter = lower_bound(hand.begin(), hand.end(), lastTile);
	if (iter != hand.end() && *iter == lastTile) {
		int index = iter - hand.begin();
		try {
			hand.at(index).setNumber(hand.back().getNumber());
			hand.at(index).setSuit(hand.back().getSuit());
		}
		catch (...) {
			throw runtime_error("Could not set the number or suit on a tile.");
		}
		hand.pop_back();
		sort(hand.begin(), hand.end());
	}

	tile curTile;
	for (int i = 0; i < hand.size(); ++i) {
		curTile = hand.at(i);
		if (curTile.getSuit() == lastTile.getSuit()) {
			if (lastTile.getNumber() + 1 == curTile.getNumber()) {
				if (i + 1 >= hand.size()) {
					return false;
				}
				// Check if the lastTile is the start of a straight
				else if ((curTile.getSuit() == hand.at(i + 1).getSuit())
					&& (curTile.getNumber() + 1 == hand.at(i + 1).getNumber())) {
					return true;
				}
				else {
					return false;
				}
			}
			else if (lastTile.getNumber() == curTile.getNumber() + 1) {
				if ((i + 1 >= hand.size()) || (i - 1 < 0)) {
					return false;
				}
				// Check if the lastTile is the end of a straght
				else if ((curTile.getSuit() == hand.at(i - 1).getSuit())
					&& (curTile.getNumber() - 1 == hand.at(i - 1).getNumber())) {
					return true;
				}
				// Check if the lastTle is the middle of a staight
				else if ((lastTile.getSuit() == hand.at(i + 1).getSuit())
					&& (lastTile.getNumber() + 1 == hand.at(i + 1).getNumber())) {
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

void table::chi() {
	if (!canChi()) {
		return;
	}

	drawnTile = discardPile.at(discardPile.size() - 1);
	drawnTile.setShown(true);
	discardPile.pop_back();
}

void table::revealSet(vector<tile>* hand) {
	// 0 = prevTile, 1 = nextTile, 2 = nextNextTile
	int tileIndices[3] = {0, 0, 0};
	tile prevTile;
	tile curTile;
	tile nextTile;
	tile nextNextTile;
	for (int i = 0; i < hand->size(); ++i) {
		curTile = hand->at(i);
		if (curTile.getSuit() == drawnTile.getSuit()) {
			if (drawnTile.getNumber() + 1 == curTile.getNumber()) {
				if (i + 1 >= hand->size()) {
					return;
				}
				
				for (int j = i; j + 1 < hand->size(); ++j) {
					nextTile = hand->at(j + 1);
					tileIndices[1] = j + 1;

					if (curTile != nextTile && !nextTile.isShown()) {
						break;
					}
				}
				// Check if the drawnTile is the start of a straight
				if ((curTile.getSuit() == nextTile.getSuit())
					&& (curTile.getNumber() + 1 == nextTile.getNumber())) {
					hand->at(i).setShown(true);
					hand->at(tileIndices[1]).setShown(true);
					return;
				}
				else {
					return;
				}
			}
			else if (drawnTile.getNumber() == curTile.getNumber() + 1) {
				bool atStart = i - 1 < 0;
				bool atEnd = i + 1 >= hand->size();

				for (int j = i; j - 1 >= 0; --j) {
					prevTile = hand->at(j - 1);
					tileIndices[0] = j - 1;

					if (curTile != prevTile && !prevTile.isShown()) {
						break;
					}
				}
				for (int j = i; j + 1 < hand->size(); ++j) {
					nextNextTile = hand->at(j + 1);
					tileIndices[2] = j + 1;

					if (curTile != nextNextTile && !nextNextTile.isShown()) {
						break;
					}
				}
				// Check if the drawnTile is the end of a straght
				if ((curTile.getSuit() == prevTile.getSuit() && !atStart)
					&& (curTile.getNumber() - 1 == prevTile.getNumber())) {
					hand->at(i).setShown(true);
					hand->at(tileIndices[0]).setShown(true);
					return;
				}
				// Check if the drawnTile is the middle of a staight
				else if ((drawnTile.getSuit() == nextNextTile.getSuit() && !atEnd)
					&& (drawnTile.getNumber() + 1 == nextNextTile.getNumber())) {
					hand->at(i).setShown(true);
					hand->at(tileIndices[2]).setShown(true);
					return;
				}
				else {
					return;
				}
			}
		}
	}
}

bool table::checkWin() const {
	// There are two win conditions.
	// First win condition: 4 sets of 3 and a pair
	// Second win condition: 1 set of 4, 2 sets of 3, and 2 pairs

	vector<tile> hand = players.at(currentPlayer).c_getHand();
	// temporarily include the drawn tile to see if this tile makes a winning hand
	hand.push_back(drawnTile);
	// sorting makes it easier to identify a winning hand
	sort(hand.begin(), hand.end());

	tile prevTile;
	tile curTile;
	int pair = 0;
	int triple = 0;
	int quad = 0;
	int match = false; //Is this building a matching set
	int setCount = 1; //The first tile is starting a set
	// start at 1 because 0 is the start of the set
	for (int i = 1; i < hand.size(); ++i) {
		prevTile = hand.at(i - 1);
		curTile = hand.at(i);
		if (prevTile.getSuit() == curTile.getSuit()) {
			if (prevTile.getNumber() + 1 == curTile.getNumber()) {
				++setCount;
				if (setCount == 3) {
					++triple;
					setCount = 1;
					++i;
				}
			}
			else if (prevTile.getNumber() == curTile.getNumber()) {
				if (setCount > 1 && !match) {
					if (setCount == 2) {
						++pair;
						setCount = 0;
						if (pair > 2) {
							return false;
						}
					}
					else if (setCount == 3) {
						++triple;
						setCount = 0;
					}
					else if (setCount == 4) {
						++quad;
						setCount = 0;
					}
				}
				++setCount;
				if (setCount == 3 && match) {
					// Check if we have triple or quad & mark accordingly
					// Check if this is the last tile in the hand
					if (i + 1 >= hand.size()) {
						++triple;
						setCount = 0;
						match = false;
						break; // This is the last tile so exit the loop
					}
					else { // There is another next tile so check for quad
						tile nextTile = hand.at(i + 1);
						if ((curTile.getSuit() == nextTile.getSuit())
							&& (curTile.getNumber() == nextTile.getNumber())) {
							++quad;
							setCount = 1;
							match = false;
							i += 2; // This can go beyond the hand size, but the loop check will prevent out of bounds indices
							continue;
						}
						else {
							// Check for edge case to determine if it's a double next to a straight or a triple next to a straight
							if ((curTile.getSuit() == nextTile.getSuit())
								&& (curTile.getNumber() + 1 == nextTile.getNumber())) {
								int tempTriple = 0;
								int tempSetCount = 2;
								tile tempPrevTile;
								tile tempCurTile;
								for (int j = i + 2; j < hand.size(); ++j) {
									tempPrevTile = hand.at(j - 1);
									tempCurTile = hand.at(j);
									if ((tempPrevTile.getSuit() == tempCurTile.getSuit())
										&& (tempPrevTile.getNumber() + 1 == tempCurTile.getNumber())) {
										++tempSetCount;
										if (tempSetCount == 3) {
											++tempTriple;
											tempSetCount = 0;
										}
									}
									else {
										if (tempSetCount == 0 && tempTriple > 0) {
											triple += tempTriple;
											setCount = 1;
											++pair;
											match = false;
											i = j;
											break;
										}
										else if (tempSetCount == 1 && tempTriple > 0) {
											// Starting assumption was a pair and a straight,
											// but actually was two triples
											triple += tempTriple + 1;
											setCount = 0;
											match = false;
											i = j - 1;
											++i;
											break;
										}
										else {
											++pair;
											triple += tempTriple;
											setCount = 1;
											match = false;
											i = j - tempSetCount;
											break;
										}
									}
								}
							}
							else {
								++triple;
								setCount = 1;
								match = false;
								++i;
							}
							continue;
						}
					}
				}
				else if (setCount == 2 && !match) {
					if (i + 1 >= hand.size()) {
						++pair;
						setCount = 1;
						if (pair > 2) {
							return false;
						}
						++i;
						break; // This is the last tile so exit the loop
					}
					else { // There is another next tile so check for triple
						tile nextTile = hand.at(i + 1);
						// Check for edge case to determine if it's a double next to a straight or a triple next to a straight
						if ((curTile.getSuit() == nextTile.getSuit())
							&& (curTile.getNumber() + 1 == nextTile.getNumber())) {
							++pair;
							match = false;
							setCount = 1;
							if (pair > 2) {
								return false;
							}
							++i;
							continue;
						}
						else if ((curTile.getSuit() == nextTile.getSuit())
							&& (curTile.getNumber() == nextTile.getNumber())) {
							int tempTriple = 0;
							int tempSetCount = 1;
							tile tempPrevTile;
							tile tempCurTile;
							for (int j = i + 2; j < hand.size(); ++j) {
								tempPrevTile = hand.at(j - 1);
								tempCurTile = hand.at(j);
								if ((tempPrevTile.getSuit() == tempCurTile.getSuit())
									&& (tempPrevTile.getNumber() + 1 == tempCurTile.getNumber())) {
									++tempSetCount;
									if (tempSetCount == 3) {
										++tempTriple;
										tempSetCount = 0;
									}
								}
								if ((tempPrevTile.getSuit() == tempCurTile.getSuit())
									&& (tempPrevTile.getNumber() == tempCurTile.getNumber())) {
									// Could be a quad or a triple with a straight
									int tempTriple2 = 0;
									int tempSetCount2 = 1;
									tile tempPrevTile2;
									tile tempCurTile2;
									for (int k = j + 1; k < hand.size(); ++k) {
										tempPrevTile2 = hand.at(k - 1);
										tempCurTile2 = hand.at(k);
										if ((tempPrevTile.getSuit() == tempCurTile.getSuit())
											&& (tempPrevTile.getNumber() + 1 == tempCurTile.getNumber())) {
											// Check if triple and straights or quad and straights
											++tempSetCount2;
											if (tempSetCount2 == 3) {
												++tempTriple2;
												tempSetCount2 = 0;
											}
										}
										else {
											if (tempSetCount2 == 2 && tempTriple2 > 0) {
												++quad;
												triple += tempTriple + tempTriple2;
												setCount = 1;
												match = false;
												i = k - 1;
												break;
											}
											else {
												++quad;
												triple += tempTriple + tempTriple2;
												setCount = 1;
												match = false;
												i = k;
												break;
											}
										}
									}
								}
								else {
									if (tempSetCount == 0 && tempTriple > 0) {
										triple += tempTriple;
										setCount = 1;
										++pair;
										match = false;
										i = j;
										break;
									}
									else if (tempSetCount == 1 && tempTriple > 0) {
										// Starting assumption was a pair and a straight,
										// but actually was two triples
										triple += tempTriple + 1;
										setCount = 0;
										match = false;
										i = j - 1;
										++i;
										break;
									}
									else {
										++triple;
										triple += tempTriple;
										setCount = 1;
										match = false;
										i = j - tempSetCount;
										++i;
										break;
									}
								}
							}
							if (i + 2 < hand.size()) {
								// If the for loop was entered then don't update match to true
								continue;
							}
						}
					}
				}
				match = true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	if (pair == 1 && (triple == 4 && quad == 0)) {
		return true;
	}
	else if (pair == 2 && (triple == 2 && quad == 1)) {
		return true;
	}

	return false;
}

tile table::getDrawnTile() const {
	return drawnTile;
}

player* table::getPlayer(int seatPosition) {
	if (seatPosition < 0 || seatPosition >= players.size()) {
		throw out_of_range("Index out of range");
	}

	return &players.at(seatPosition);
}

vector<tile>* table::getPlayerHand(int seatPosition) {
	vector<tile>* hand;
	if (seatPosition < 0 || seatPosition >= players.size()) {
		throw out_of_range("Index out of range");
	}

	return players.at(seatPosition).getHand();
}

deck* table::getDeck() {
	return &walls;
}

int table::getCurrentPlayer() const {
	return currentPlayer;
}