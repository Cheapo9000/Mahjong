/**
 * @file mahjong.cpp
 * @brief A local console based mahjong game.
 *
 * A console based mahjong game with up to 4 players. Players must have integrity to not scroll up when playing and must switch to the next player in the 5 second interval. Other players should not be looking at the screen when it is a differnt player's turn.
 * COMING UPDATES:
 * Win states
 * PENG and GONG screen (includes peeking at your hand option)
 * Network play
 */
#include <iostream>
#include <regex>
#include <thread>
#include <chrono>
#include "table.h"
using namespace std;

table board;

static bool isNumber(const string& str) {
    regex numberPattern("^-?\\d+$");
    return regex_match(str, numberPattern);
}

void countFiveSeconds() {
    for (int i = 1; i <= 5; ++i) {
        this_thread::sleep_for(chrono::seconds(1));
        cout << i << " ";
    }
    cout << endl;
}

int main()
{
    int playerCount = 0;
    string input;

    // Input player names
    cout << "Input the names of players one at a time followed by ENTER. Minimum of 2 players and maximum of 4 players or input DONE if you are finished inputting names." << endl;
    do {
        cout << "Input your name: ";
        getline(cin, input);
        if (!input.empty()) {
            if (playerCount > 1 && input == "DONE") {
                break;
            }
            board.seatPlayer(input);
            ++playerCount;
        }
    } while (playerCount < 4);

    board.welcomePlayers();

    cout << "Dealing your hands." << endl;

    try {
        board.dealHands();
    }
    catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    cout << "Hands have been dealt." << endl;

    cout << "Press ENTER to start the game!" << endl;

    getline(cin, input);

    string playerName = "";
    int curPlayer = 0;
    regex keepPattern("k", regex_constants::icase);
    regex discardPattern("d", regex_constants::icase);
    regex yesPattern("y", regex_constants::icase);
    regex noPattern("n", regex_constants::icase);
    bool hasKept;

    // Main game loop continues until players run out of tiles.
    while (board.getDeck()->getTiles()->size() > 0) {
        curPlayer = board.getCurrentPlayer();
        hasKept = false;
        try {
            playerName = board.getPlayer(curPlayer)->getName();
            cout << playerName << ", it's your turn!" << endl;
        }
        catch (const std::exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }
        cout << "Discard pile: " << endl;
        board.displayDiscardPile();

        if (board.canChi()) {
            board.displayTable(curPlayer);

            do {
                cout << "Would you like to Chi? Input Y as yes and N as no and press ENTER to submit your choice." << endl;
                getline(cin, input);

                if (regex_search(input, yesPattern)) {
                    board.chi();
                    hasKept = true;

                    cout << "Discard pile: " << endl;
                    board.displayDiscardPile();
                }
                else if (regex_search(input, noPattern)) {
                    board.drawTile();
                    cout << "Drawing tile: " << board.getDrawnTile().getDisplayValue() << endl;
                    break;
                }
                else{
                    cout << "Invalid input provided." << endl;
                }
            } while (true);
        }
        else {
            board.drawTile();
            cout << "Drawing tile: " << board.getDrawnTile().getDisplayValue() << endl;
        }

        board.displayTable(curPlayer);

        if (board.checkWin()) {
            cout << playerName << " WINS!" << endl;
            return 0;
        }

        // Player chooses to keep or discard drawn tile.
        do {
            if (!hasKept) {
                cout << "Input K to keep or D to discard and press ENTER to submit your choice." << endl;
                getline(cin, input);
            }
            else {
                input = "k";
            }

            if (regex_search(input, keepPattern)) {
                // Player chooses which tile in their hand to discard. They must discard.
                do {
                    cout << "Which tile would you like to discard? Input the number and press ENTER to submit your choice." << endl;
                    getline(cin, input);

                    if (isNumber(input)) {
                        int index = stoi(input) - 1;
                        int playerHandSize;
                        try {
                            playerHandSize = board.getPlayerHand(curPlayer)->size();
                        }
                        catch (const std::exception& e) {
                            cerr << "Error: " << e.what() << endl;
                            return 1;
                        }
                        if (index < 0 || index > playerHandSize) {
                            cout << "Invalid input provided. You must choose a number from the list of valid indices." << endl;
                        }
                        else {
                            try {
                                if (!board.discardTile(index)) {
                                    cout << "Invalid tile selected. You must choose a tile that is not visible." << endl;
                                    continue;
                                }
                            }
                            catch (const std::exception& e) {
                                cerr << "Error: " << e.what() << endl;
                                return 1;
                            }
                            board.keepDrawn();
                            board.displayTable(curPlayer);
                            cout << "Press ENTER to end your turn." << endl;
                            getline(cin, input);
                            // TODO: Add a PENG and GONG option screen
                            cout << "Changing to next player in 5 seconds." << endl;
                            countFiveSeconds();
                            cout << "\033[2J\033[1;1H"; // clear the screen
                            board.nextPlayer();
                            break;
                        }
                    }
                    else {
                        cout << "Invalid input provided. Value must be a whole number." << endl;
                    }
                } while (true);
                break;
            }
            else if (regex_search(input, discardPattern)) {
                board.discardDrawn();
                board.displayTable(curPlayer);
                cout << "Press ENTER to end your turn." << endl;
                getline(cin, input);
                // TODO: Add a PENG and GONG option screen
                cout << "Changing to next player in 5 seconds." << endl;
                countFiveSeconds();
                cout << "\033[2J\033[1;1H"; // clear the screen
                board.nextPlayer();
                break;
            }
            else {
                cout << "Invalid input provided." << endl;
            }
        } while (true);

    }

    cout << "You have run out of tiles and there was no winner found so it's a draw!" << endl;
}