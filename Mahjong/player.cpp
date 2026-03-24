#include "player.h"

player::player() {
	this->reset();
}

void player::setName(string name) {
	this->name = name;
}

void player::setSeatPos(int position) {
	this->position = position;
}

void player::setHand(vector<tile>& hand) {
	this->hand = hand;
}

void player::setShownTiles(bool shown) {
	this->hasShown = shown;
}

void player::reset() {
	this->name = "";
	this->position = -1;
	this->hand = vector<tile>();
	this->hasShown = false;
}

string player::getName() const {
	return this->name;
}

int player::getSeatPos() const {
	return this->position;
}

int player::getHandSize() const {
	return this->hand.size();
}

vector<tile> player::c_getHand() const {
	return this->hand;
}

vector<tile>* player::getHand() {
	return &(this->hand);
}

bool player::hasShownTiles() {
	return hasShown;
}