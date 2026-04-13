/**
 * @file tile.h
 * @author Tim Lake
 * @copyright 2026
 * @brief Represents a Mahjong tile with suit and number.
 *
 * Provides methods for setting, getting, and displaying tile values.
 */
#ifndef TILE_H
#define TILE_H

#include <stdexcept>
#include <string>
using namespace std;

enum suit {
    none = 0,
    circle,
    bamboo,
    character,
    wind,
    dragon
};

/**
 * @class tile
 * @brief Represents a single tile with a suit and numeric value.
 *
 * Provides methods for setting, getting, and displaying tile values.
 */
class tile {

    int num;
    suit kind;
    bool shown;
    const int maxNum = 10;

public:
    /**
     * @brief Sets the initial values to zero and none states respectively.
     */
    tile()
    {
        this->reset();
    }

    /**
     * @brief Sets the initial values to the kind and num values respectively.
     * If the values are outside the range of valid suits and/or numbers then a suit of none and/or zero will be given to the tile.
     */
    tile(suit kind, int num) {
        if (kind >= none && kind <= dragon) {
            this->kind = kind;
        }
        else {
            this->kind = none;
        }
        if (num > 0 && num < maxNum) {
            this->num = num;
        } else {
            this->num = 0;
        }
        this->shown = false;
    }

    /**
     * @brief Sets the initial values to the same as the provided tile.
     */
    tile(const tile& oTile) {
        this->num = oTile.getNumber();
        this->kind = oTile.getSuit();
        this->shown = oTile.isShown();
    }

    /**
     * @brief Assignment operator overloaded to ensure the copy is handled correctly.
     * @return A reference to a tile with the values that have been assigned from the other tile.
     */
    tile& operator=(const tile& other) {
        this->num = other.getNumber();
        this->kind = other.getSuit();
        this->shown = other.isShown();

        return *this;
    }

    /**
     * @brief Less than operator overloaded to ensure the comparison is handled correctly.
     * @return A bool with the value true if the kind and/or number is less than this tile.
     */
    bool operator<(const tile& other) const
    {
        if (this->isShown() != other.isShown())
            return !this->isShown();

        if (this->kind != other.kind)
            return this->kind < other.kind;

        return this->num < other.num;
    }

    /**
     * @brief Equality operator overloaded to ensure the comparison is handled correctly.
     * @return A bool with the value true if the other tile is equal to this tile.
     */
    bool operator==(const tile& other) const
    {
        return this->kind == other.kind && this->num == other.num;
    }

    /**
     * @brief Inequality operator overloaded to ensure the comparison is handled correctly.
     * @return A bool with the value true if the other tile isn't equal to this tile.
     */
    bool operator!=(const tile& other) const
    {
        return this->kind != other.kind || this->num != other.num;
    }

    /**
     * @brief Sets the suit representation of a tile.
     * @throw out_of_range exception if the kind is above or below the valid suits.
     */
    void setSuit(suit kind) {
        if (kind >= none && kind <= dragon) {
            this->kind = kind;
        }
        else {
            throw out_of_range("suit out of range");
        }
    }

    /**
     * @brief Sets the number representation of the tile.
     * @throw out_of_range exception if the num is at or above maxNum or below zero.
     */
    void setNumber(int num) {
        if (num >= 0 && num < maxNum) {
            this->num = num;
        }
        else {
            throw out_of_range("num out of range");
        }
    }

    /**
     * @brief Sets whether the tile should be visible to other players or not.
     */
    void setShown(bool show) {
        this->shown = show;
    }

    /**
     * @brief Sets the tile values to initial state of zero and none respectively.
     */
    void reset() {
        this->num = 0;
        this->kind = none;
        this->shown = false;
    }

    /**
     * @brief Gets an enum representation of the suit of the tile.
     * @return An enum representing the suit of the tile.
     */
    suit getSuit() const {
        return this->kind;
    }

    /**
     * @brief Gets an integer of the number tile.
     * @return An integer of the number tile. For Winds and Dragons these correspond to directions and colors.
     * Winds: 0 = E, 1 = S, 2 = W, 3 = N
     * Dragons: 0 = R, 1 = G, 2 = W
     */
    int getNumber() const {
        return this->num;
    }

    /**
     * @brief Gets whether or not the tile should be visibnle to other players
     * @return A bool containing true if it should be shown, false otherwise.
     */
    bool isShown() const {
        return this->shown;
    }

    /**
     * @brief Gets a string representation of the tile.
     * @return A formatted string representing the tile (e.g., "C5", "WE").
     * If the tile is in the default state then an empty string is returned.
     */
    string getDisplayValue() const {
        string display = "";
        switch (this->kind)
        {
        case none:
            // do nothing because we have no value. This shouldn't happen
            return display;
            break;
        case circle:
            display += "C";
            break;
        case bamboo:
            display += "B";
            break;
        case character:
            // Display as N (number) to not confuse the suits
            display += "N";
            break;
        case wind:
            display += "W";
            if (this->num == 1) {
                display += "E";
            }
            else if (this->num == 2) {
                display += "S";
            }
            else if (this->num == 3) {
                display += "W";
            }
            else if (this->num == 4) {
                display += "N";
            }
            return display;
            break;
        case dragon:
            display += "D";
            if (this->num == 1) {
                display += "R";
            }
            else if (this->num == 2) {
                display += "G";
            }
            else if (this->num == 3) {
                display += "W";
            }
            return display;
            break;
        default:
            break;
        }
        display += to_string(this->num);

        return display;
    }
};

#endif // TILE_H