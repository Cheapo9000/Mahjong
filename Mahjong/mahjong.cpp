/**
 * @name Mahjong
 * @file mahjong.cpp
 * @author Tim Lake
 * @copyright 2026
 * @brief A local console based mahjong game.
 *
 * A console based mahjong game with up to 4 players. Players must have integrity to not scroll up when playing and must switch to the next player in the 5 second interval. Other players should not be looking at the screen when it is a differnt player's turn.
 * COMING UPDATES:
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
    regex pengPattern("p", regex_constants::icase);
    regex gongPattern("g", regex_constants::icase);
    bool hasKept;
    int pengOrGong;

    // Main game loop continues until players run out of tiles.
    while (board.getDeck()->getTiles()->size() > 0) {
        curPlayer = board.getCurrentPlayer();
        hasKept = false;

        if (board.hasDiscardedTiles()) {
            pengOrGong = board.canPengOrGong();
            board.displayTable();
            cout << "Discard pile: " << endl;
            board.displayDiscardPile();

            cout << "Would any players like to Peng or Gong the last tile? (Peng means the tile will complete a set of 3 and Gong will complete a set of 4.)" << endl;
            board.displayPengOrGongPlayers();
            cout << "[3] Clear screen" << endl;
            cout << "[4] Peek at hand" << endl;
            cout << "[5] Exit Menu" << endl;
            do {
                getline(cin, input);

                if (isNumber(input)) {
                    if (input == "5") {
                        cout << "\033[2J\033[1;1H"; // clear the screen
                        break;
                    }
                    else if (input == "3") {
                        cout << "\033[2J\033[1;1H"; // clear the screen
                        board.displayTable();
                        cout << "Discard pile: " << endl;
                        board.displayDiscardPile();

                        cout << "Would any players like to Peng or Gong the last tile? (Peng means the tile will complete a set of 3 and Gong will complete a set of 4.)" << endl;
                        board.displayPengOrGongPlayers();
                        cout << "[3] Clear screen" << endl;
                        cout << "[4] Peek at hand" << endl;
                        cout << "[5] Exit Menu" << endl;
                    }
                    // Take a peek at your hand, but the code doesn't know which player's hand to show so show some options!
                    else if (input == "4") {
                        cout << "\033[2J\033[1;1H"; // clear the screen
                        board.displayPengOrGongPlayers();
                        cout << "[4] Exit Peek Menu" << endl;

                        do {
                            getline(cin, input);

                            if (isNumber(input)) {
                                if (input == "4") {
                                    cout << "\033[2J\033[1;1H"; // clear the screen
                                    board.displayTable();
                                    cout << "Discard pile: " << endl;
                                    board.displayDiscardPile();

                                    cout << "Would any players like to Peng or Gong the last tile? (Peng means the tile will complete a set of 3 and Gong will complete a set of 4.)" << endl;
                                    board.displayPengOrGongPlayers();
                                    cout << "[3] Clear screen" << endl;
                                    cout << "[4] Peek at hand" << endl;
                                    cout << "[5] Exit Menu" << endl;
                                    break;
                                }
                                else {
                                    board.displayTable(stoi(input));
                                    cout << "Input 4 and press ENTER to exit when you are done looking at your hand." << endl;
                                }
                            }
                            else {
                                cout << "Must input only a valid whole number!" << endl;
                            }
                        } while (true);
                    }
                    else {
                        // Attempt to peng or gong for the chosen player
                        int pengOrGong = 0;
                        try {
                            pengOrGong = board.pengOrGong(stoi(input));
                        }
                        catch (...) {
                            // Do nothing because this invalid input shouldn't crash the game.
                            cout << "Must be a valid seat position!" << endl;
                        }
                        // Peng
                        if (pengOrGong == 1) {
                            cout << "\033[2J\033[1;1H"; // clear the screen
                            cout << "Peng!" << endl;
                            board.setCurrentPlayer(stoi(input), true);
                            hasKept = true;
                            break;
                        }
                        // Gong
                        else if (pengOrGong == 2) {
                            bool wantsPeng = false;
                            cout << "Do you want to Peng or Gong? Input P to Peng or G to Gong and press ENTER:" << endl;
                            do {
                                getline(cin, input);

                                if (regex_search(input, pengPattern)) {
                                    wantsPeng = true;
                                    board.setCurrentPlayer(stoi(input), true);
                                    board.setWantsPeng(curPlayer);
                                    hasKept = true;
                                    break;
                                }
                                else if (regex_search(input, gongPattern)) {
                                    wantsPeng = false;
                                    board.setCurrentPlayer(stoi(input), true);
                                    hasKept = true;
                                    break;
                                }
                                else {
                                    cout << "Invalid input! You must input a P to Peng or G to Gong and press ENTER." << endl;
                                }
                            } while (true);

                            cout << "\033[2J\033[1;1H"; // clear the screen
                            if (wantsPeng) {
                                cout << "Peng!" << endl;
                            }
                            else {
                                cout << "Gong!" << endl;
                            }
                        }
                        else {
                            cout << "You cannot peng or gong!" << endl;
                        }
                    }
                }
                else {
                    cout << "Must input only a valid whole number!" << endl;
                }
            } while (true);
        }

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

        if (!hasKept) {
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
                        break;
                    }
                    else if (regex_search(input, noPattern)) {
                        board.drawTile();
                        cout << "Drawing tile: " << board.getDrawnTile().getDisplayValue() << endl;
                        break;
                    }
                    else {
                        cout << "Invalid input provided." << endl;
                    }
                } while (true);
            }
            else {
                board.drawTile();
                cout << "Drawing tile: " << board.getDrawnTile().getDisplayValue() << endl;
            }
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