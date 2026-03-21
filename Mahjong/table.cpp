#include "table.h"
#include <iostream>

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
	hand->push_back(drawnTile);
	sort(hand->begin(), hand->end());
	drawnTile.reset();
}

void table::discardTile(int index) {
	vector<tile>* hand = players.at(currentPlayer).getHand();
	if (index < 0 || index >= hand->size()) {
		throw out_of_range("Index out of range");
	}

	discardPile.push_back(hand->at(index));
	try {
		hand->at(index).setNumber(hand->back().getNumber());
		hand->at(index).setSuit(hand->back().getSuit());
	}
	catch (...) {
		throw runtime_error("Could not set the number or suit on a tile.");
	}
	hand->pop_back();
	sort(hand->begin(), hand->end());
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
	player player = players.at(seatPosition);
	vector<tile> hand = player.c_getHand();
	for (int i = 0; i < hand.size(); ++i) {
		if (i < 9) {
			cout << "| " << i + 1;
		} else{
			cout << "|" << i + 1;
		}
	}
	cout << "|" << endl;
	for (int i = 0; i < hand.size(); ++i) {
		cout << "|" << hand.at(i).getDisplayValue();
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

bool table::checkWin() const {
	// TODO: Check win condition
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