#include "deck.h"

void deck::build() {
    tiles.clear();
    tiles.shrink_to_fit();
    int numMax = 9;
    for (int kind = 1; kind <= dragon; ++kind) {
        if (kind == wind) {
            numMax = 4;
        }
        else if (kind == dragon) {
            numMax = 3;
        }
        for (int num = 1; num <= numMax; ++num) {
            for (int dup = 0; dup < 4; ++dup) {
                tiles.push_back(tile((suit)kind, num));
            }
        }
    }
    // Enforce the correct tile count
    if (tiles.size() != maxTiles) {
        tiles.clear();
    }
}

deck::deck() {
    tiles = vector<tile>();
    this->build();
}

void deck::reset(vector<player>* players, vector<tile>* discarded) {
    if (players != nullptr) {
        vector<tile>* hand;
        for (int i = 0; i < players->size(); ++i) {
            hand = players->at(i).getHand();
            this->tiles.insert(this->tiles.end(), hand->begin(), hand->end());
            players->at(i).reset();
        }
    }
    if (discarded != nullptr) {
        this->tiles.insert(this->tiles.end(), discarded->begin(), discarded->end());
        discarded->clear();
        discarded->shrink_to_fit();
    }
}

void deck::addTiles(vector<tile>& tiles) {
    this->tiles.insert(this->tiles.end(), tiles.begin(), tiles.end());
}

void deck::setTiles(vector<tile>& tiles) {
    this->tiles=tiles;
}

int deck::getHandSize() const {
    return this->handSize;
}

vector<tile>* deck::getTiles() {
    return &(this->tiles);
}

vector<tile> deck::getHand() {
    random_device rd;
    mt19937 gen(rd());
    vector<tile> hand = vector<tile>();
    int index;
    for(int i=0; i < handSize; ++i) {
        index = uniform_int_distribution<>(0, tiles.size() - 1)(gen);
        hand.push_back(tiles.at(index));
        try {
            tiles.at(index).setNumber(tiles.back().getNumber());
            tiles.at(index).setSuit(tiles.back().getSuit());
        }
        catch (...) {
            throw runtime_error("Could not set the number or suit on a tile.");
        }
        tiles.pop_back();
    }
    return hand;
}

tile deck::drawTile() {
    random_device rd;
    mt19937 gen(rd());
    int index = uniform_int_distribution<>(0, tiles.size() - 1)(gen);
    tile drawn = tiles.at(index);
    tiles.at(index).setNumber(tiles.back().getNumber());
    tiles.at(index).setSuit(tiles.back().getSuit());
    tiles.pop_back();
    return drawn;
}