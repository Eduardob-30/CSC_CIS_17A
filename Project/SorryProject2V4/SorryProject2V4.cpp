/* 
 * File:   SorryProject2V4.cpp
 * Author: eddiebrito
 *
 * Created on May 30, 2025, 1:38 PM
 */
// System Libraries
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <map>
#include "Game_Classes.h"
#include "Counter.h" 
using namespace std;
//total move counting global
Counter<int> totalMoves; 
int PlayerV4::plyrCnt = 0;
// Function Prototypes
void showSortedStats(PlayerV4*[], int);
int draw(CardDeck &);
//mechanics
void moveBot(PlayerV4*[], int, int);
void moveBotHard(PlayerV4*[], int, int);         //Harder difficulty
void move(PlayerV4*[], int, int, Counter<int>&);
void slide(PawnV4 &);
void bump(PlayerV4*[], int, int);
bool isDone(PlayerV4*);
void undoMove(PlayerV4* players[], int currIdx);
//outputs
void showBoard(PlayerV4*[], int);
void showStats(PlayerV4*[], int, int, const Counter<int>&);
//file I/O
void saveGame(PlayerV4*[], int, string);
bool loadGame(PlayerV4*[], int &, string);
// Main
int main() {
    srand(static_cast<unsigned int>(time(0)));

    PlayerV4* players[MAX_PLAYERS] = {nullptr};
    int plyrCnt = 2;
    int turn = 0;
    char choice;
    bool hardBot = false;  // Track bot difficulty

    int menuChoice;
    do {
        cout << "===== Sorry! Game Menu =====\n";
        cout << "1. View Rules\n";
        cout << "2. Load Saved Game\n";
        cout << "3. Start New Game\n";
        cout << "4. Exit\n";
        cout << "Enter your choice (1–4): ";
        cin >> menuChoice;
        cin.ignore();

        switch (menuChoice) {
            case 1:
                cout << "============================================================\n";
                cout << "Hello! Welcome to the Sorry! Game\n";
                cout << "============================================================\n";
                cout << "Objective: Get ALL your pawns to the HOME base to win!\n";
                cout << "Rules:\n";
                cout << "- You must draw a 1 OR 2 to leave START base.\n";
                cout << "- After leaving START, move forward the number of spaces\n";
                cout << "  shown on your next drawn card (1–12).\n";
                cout << "- Landing on a [=>] slide moves you 3 more spaces.\n";
                cout << "- If you're close to the finish you must pull the exact number\n";
                cout << "  of spaces it takes for the finish in order to get to HOME. \n";
                cout << "- If you land on an opponent’s pawn, they go back to START.\n";
                cout << "- If you draw a 13, it's a SORRY! card. Take over an opponent's spot!\n";
                cout << "Good luck playing the Sorry! game\n";
                cout << "============================================================\n\n";
                break;
            case 2: {
                cout << "Load saved game?\n";
                cout << "Enter save file name to load: ";
                string fileName;
                getline(cin, fileName);
                bool loaded = loadGame(players, turn, fileName);
                if (!loaded) {
                    cout << "Save file not found. Starting new game.\n";
                    menuChoice = 3;
                } 
                else {
                    menuChoice = 0;
                }
                break;
            }
            case 3:
                break;
            case 4:
                cout << "Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } 
    while (menuChoice != 3 && menuChoice != 0);

    if (menuChoice == 3) {
        do {
            cout << "How many players? (1 to 4): ";
            cin >> plyrCnt;
            cin.ignore();
            if (plyrCnt < 1 || plyrCnt > 4)
                cout << "Invalid number. Please enter between 1 and 4.\n";
        } while (plyrCnt < 1 || plyrCnt > 4);

        if (plyrCnt == 1) {
            string name;
            cout << "Enter your name: ";
            getline(cin, name);
            players[0] = new PlayerV4();
            players[0]->setName(name);
            players[0]->setSym('A');

            // Ask for difficulty
            int diff;
            do {
                cout << "Choose bot difficulty:\n";
                cout << "1. Regular\n";
                cout << "2. Hard\n";
                cout << "Enter choice (1 or 2): ";
                cin >> diff;
                cin.ignore();
                if (diff == 2) hardBot = true;
                else if (diff != 1) cout << "Invalid. Try again.\n";
            } 
            while (diff != 1 && diff != 2);

            players[1] = new Bot();
            players[1]->setSym('B');
            plyrCnt = 2;
        } 
        else {
            for (int i = 0; i < plyrCnt; i++) {
                string name;
                cout << "Enter name for Player " << i + 1 << ": ";
                getline(cin, name);
                players[i] = new PlayerV4();
                players[i]->setName(name);
                players[i]->setSym('A' + i);
            }
        }
    }
    //drawing card
    CardDeck deck;
    while (true) {
        int currIdx = turn % plyrCnt;
        PlayerV4* curr = players[currIdx];
        cout << "\nTurn: " << curr->getName() << " (" << curr->getSym() << ")\n";
        showBoard(players, B_SIZE);
        int card = draw(deck);
        cout << "Card drawn: " << card << "\n";

        if (curr->isBot()) {
            if (hardBot) moveBotHard(players, currIdx, card);
            else moveBot(players, currIdx, card);
        } 
        else {
            move(players, currIdx, card, totalMoves);
        }
        //checking if a player won
        if (isDone(players[currIdx])) {
            cout << "\n" << curr->getName() << " wins! All pawns are HOME!\n";
            showStats(players, turn + 1, plyrCnt, totalMoves);
            showSortedStats(players, plyrCnt);
            break;
        }
        //options after each round
        char cont;
        cout << "\nOptions:\n";
        cout << "[s] Save and quit\n";
        cout << "[u] Undo last move\n";
        cout << "[q] Quit without saving\n";
        cout << "[c] Continue playing\n";
        cout << "Enter your choice: ";
        cin >> cont;
        cin.ignore();
        //saving
        if (cont == 's' || cont == 'S') {
            string fileName;
            cout << "Enter save file name (no extension): ";
            getline(cin, fileName);
            saveGame(players, turn, fileName + ".bin");
            cout << "Game saved.\n";
            break;
        } 
        //quiting game
        else if (cont == 'q' || cont == 'Q') {
            cout << "Exiting without saving...\n";
            break;
        }
        //undoing
        else if (cont == 'u' || cont == 'U') {
            undoMove(players, currIdx);
            if (curr->isBot()) {
                if (hardBot) moveBotHard(players, currIdx, card);
                else moveBot(players, currIdx, card);
            } 
            //if nothing else applies then activate move function
            else {
                move(players, currIdx, card, totalMoves);
            }
        }
        //incrementing turn
        turn++;
    }
    for (int i = 0; i < MAX_PLAYERS; ++i)
        delete players[i];

    return 0;
}
// Function Implementations
/*
 * @brief Draws a card from the deck.
 * @param deck Reference to the card deck.
 * @return The card number drawn.
 */
int draw(CardDeck &deck) {
    return deck.draw();
}
/*
 * @brief Checks if the pawn lands on a slide and advances its position.
 * @param p Reference to the pawn.
 */
void slide(PawnV4 &p) {
    for (int i = 0; i < SLIDE_COUNT; i++) {
        if (p.getPos() == SLIDE_STARTS[i]) {
            p.setPos(p.getPos() + SLIDE_LEN);
            cout << "Landed on slide! New position: " << p.getPos() << "\n";
        }
    }
}
/*
 * @brief Sends opponent's pawn back to START if on the given position.
 * @param players array of all players.
 * @param currIdx Index of current player.
 * @param pos Position to check for bumping.
 */
void bump(PlayerV4* players[], int currIdx, int pos) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (i == currIdx) continue;
        for (int j = 0; j < P_CNT; j++) {
            if (players[i] && players[i]->getPawn(j).getPos() == pos && !players[i]->getPawn(j).isHome()) {
                players[i]->getPawn(j).sendStart();
                cout << "Bumped Player " << players[i]->getSym() << "'s Pawn " << j + 1 << "!\n";
            }
        }
    }
}
/*
 * @brief Checks if a player has all pawns at home.
 * @param p pointer to the player.
 * @return True if all pawns are home.
 */
bool isDone(PlayerV4* p) {
    return p->allHome();
}
/*
 * @brief Displays the board with player pawn positions and slide markers.
 * @param players array of player pointers.
 * @param size total board size.
 */
void showBoard(PlayerV4* players[], int size) {
    for (int row = 0; row < 6; row++) {
        for (int i = 0; i < 10; i++) {
            int idx = row * 10 + i;
            bool found = false;
            for (int p = 0; p < MAX_PLAYERS; p++) {
                if (players[p] != nullptr) {  // ✅ Avoid nullptr access
                    for (int j = 0; j < P_CNT; j++) {
                        if (players[p]->getPawn(j).getPos() == idx) {
                            cout << "[" << players[p]->getSym() << j + 1 << "]";
                            found = true;
                        }
                    }
                }
            }
            if (!found) {
                bool isSlide = false;
                for (int s = 0; s < SLIDE_COUNT; s++)
                    if (idx == SLIDE_STARTS[s]) isSlide = true;
                if (isSlide) cout << "[=>]";
                else cout << "[--]";
            }
        }
        cout << endl;
    }
}
/*
 * @brief Saves the current game state to a binary file.
 * @param players array of player pointers.
 * @param turn current turn index.
 * @param fileName name of the save file.
 */
void saveGame(PlayerV4* players[], int turn, string fileName) {
    try {
        ofstream out(fileName.c_str(), ios::binary);
        if (!out) {
            throw "Could not open file for saving.";
        }

        out.write(reinterpret_cast<char*>(&turn), sizeof(int));
        if (!out) throw "Error writing turn data.";

        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (players[i]) {
                out.write(reinterpret_cast<char*>(players[i]), sizeof(PlayerV4));
                if (!out) throw "Error writing player data.";
            } 
            else 
            {
                PlayerV4 dummy;
                out.write(reinterpret_cast<char*>(&dummy), sizeof(PlayerV4));
                if (!out) throw "Error writing dummy player data.";
            }
        }
        out.close();
        cout << "Game saved successfully to " << fileName << "\n";
    } 
    catch (const char* msg) {
        cerr << "Save error: " << msg << "\n";
    }
}
/*
 * @brief Loads a saved game state from a binary file.
 * @param players array of player pointers to get.
 * @param turn reference to store the turn index.
 * @param fileName name of the save file.
 * @return True if load was successful, false otherwise.
 */
bool loadGame(PlayerV4* players[], int &turn, string fileName) {
    try {
        ifstream in(fileName.c_str(), ios::binary);
        if (!in) {
            throw "Could not open file for reading.";
        }

        in.read(reinterpret_cast<char*>(&turn), sizeof(int));
        in.read(reinterpret_cast<char*>(players), sizeof(PlayerV4) * MAX_PLAYERS);
        in.close();
        return true;
    } 
    catch (const char* msg) {
        cout << "Load error: " << msg << endl;
        return false;
    }
}
/*
 * @brief manages player's move logic including SORRY! card and movement rules.
 * @param players array of player pointers.
 * @param currIdx index of the current player.
 * @param card the card number drawn.
 * @param totalMoves reference to move counter.
 */
void move(PlayerV4* players[], int currIdx, int card, Counter<int>& totalMoves) {
    if (players[currIdx]->isBot()) {
        moveBot(players, currIdx, card);
        return;
    }

    PlayerV4* curr = players[currIdx];
    bool moved = false;
    bool leglExsts = false;

    for (int i = 0; i < P_CNT; i++) {
        PawnV4& p = curr->getPawn(i);
        if (p.isHome()) continue;
        if (p.getPos() == START && (card == 1 || card == 2)) leglExsts = true;
        else if (p.getPos() != START && p.getPos() + card <= B_SIZE) leglExsts = true;
    }

    if (card == 13) {
        bool slfStrt = false, oppAct = false;

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (i == currIdx || players[i] == nullptr) continue;
            for (int j = 0; j < P_CNT; j++) {
                if (!players[i]->getPawn(j).isHome() &&
                    players[i]->getPawn(j).getPos() != START)
                    oppAct = true;
            }
        }

        for (int i = 0; i < P_CNT; i++) {
            if (curr->getPawn(i).getPos() == START)
                slfStrt = true;
        }

        if (!slfStrt || !oppAct) {
            cout << "No valid Sorry! moves. Turn skipped.\n";
            return;
        }
        //for a sorry card
        int targetPly, targetPwn;
        cout << "You drew a SORRY! card!\n";
        cout << "Enter opponent player index (1-" << MAX_PLAYERS << ", not " << currIdx + 1 << "): ";
        cin >> targetPly; targetPly--;
        cout << "Enter their pawn index (1 or 2): ";
        cin >> targetPwn; targetPwn--;
        //checking for selected option for a sorry card
        if (targetPly == currIdx || targetPly < 0 || targetPly >= MAX_PLAYERS ||
            targetPwn < 0 || targetPwn >= P_CNT ||
            players[targetPly] == nullptr ||
            players[targetPly]->getPawn(targetPwn).isHome() ||
            players[targetPly]->getPawn(targetPwn).getPos() == START) {
            cout << "Invalid Sorry! action.\n";
            return;
        }
        int temp = players[targetPly]->getPawn(targetPwn).getPos();
        players[targetPly]->getPawn(targetPwn).sendStart();
        cout << "Bumped opponent's Pawn back to START!\n";
        for (int i = 0; i < P_CNT; i++) {
            if (!curr->getPawn(i).isHome() &&
                curr->getPawn(i).getPos() == START) {
                curr->getPawn(i).backup();
                curr->getPawn(i).setPos(temp);
                cout << "Your Pawn " << i + 1 << " moved to " << temp << ".\n";
                bump(players, currIdx, temp);
                slide(curr->getPawn(i));
                bump(players, currIdx, curr->getPawn(i).getPos());
                moved = true;
                break;
            }
        }
        if (moved) {
            curr->incrMoves();        
            totalMoves.add(1);         
        }
        return;
    }
    if (!leglExsts) {
        cout << "No valid moves this turn.\n";
        return;
    }
    int atmpts = 0;
    while (!moved && atmpts < P_CNT) {
        int ch;
        cout << "Choose pawn to move (1 or 2): ";
        cin >> ch;
        ch--;
        if (ch < 0 || ch >= P_CNT) {
            cout << "Invalid choice.\n";
            atmpts++;
            continue;
        }
        PawnV4& p = curr->getPawn(ch);
        if (p.isHome()) {
            cout << "That pawn is already HOME.\n";
        } 
        else if (p.getPos() == START && (card == 1 || card == 2)) {
            bool overlap = false;
            for (int k = 0; k < P_CNT; ++k) {
                if (k != ch && curr->getPawn(k).getPos() == 0) {
                    overlap = true;
                    break;
                }
            }
            if (overlap) {
                cout << "Invalid move: You already have a pawn at position 0.\n";
                return;
            }
            p.backup();
            p.setPos(0);
            cout << "Pawn " << ch + 1 << " moved from START to 0.\n";
            bump(players, currIdx, p.getPos());
            slide(p);
            bump(players, currIdx, p.getPos());
            moved = true;
        } 
        else if (p.getPos() == START) {
            cout << "You need a 1 or 2 to leave START.\n";
        } 
        else if (p.getPos() + card > B_SIZE) {
            cout << "Move too far. Try another.\n";
        } 
        else if (p.getPos() + card == B_SIZE) {
            p.backup();
            p.markHome();
            cout << "Pawn " << ch + 1 << " reached HOME!\n";
            moved = true;
        } 
        else 
        {
            int newPos = p.getPos() + card;
            bool overlap = false;
            for (int k = 0; k < P_CNT; ++k) {
                if (k != ch && curr->getPawn(k).getPos() == newPos) {
                    overlap = true;
                    break;
                }
            }
            if (overlap) {
                cout << "Invalid move: You cannot land two pawns on the same spot.\n";
                return;
            }
            p.backup();
            p.setPos(p.getPos() + card);
            cout << "Pawn " << ch + 1 << " moved to " << p.getPos() << ".\n";
            bump(players, currIdx, p.getPos());
            slide(p);
            bump(players, currIdx, p.getPos());
            moved = true;
        }
        if (moved) {
            curr->incrMoves();        
            totalMoves.add(1);        
        }
        atmpts++;
    }

    if (!moved)
        cout << "No valid moves available after your attempts.\n";
}
/**
 * @brief Restores a player's pawns to their previous positions.
 * @param players array of player pointers.
 * @param currIdx index of the current player.
 */
void undoMove(PlayerV4* players[], int currIdx) {
    PlayerV4* curr = players[currIdx];
    for (int i = 0; i < P_CNT; i++) {
        curr->getPawn(i).restore();
    }
    cout << "Undo complete: Restored previous positions of your pawns.\n";
}
/*
 * @brief Displays game statistics and player performance at end of game.
 * @param players array of player pointers.
 * @param turns total number of turns taken.
 * @param plyrCnt number of players.
 * @param totalMoves reference to move counter.
 */
void showStats(PlayerV4* players[], int turns, int plyrCnt, const Counter<int>& totalMoves) {
    cout << "\n~~~~~ Game Summary ~~~~~\n";
    for (int i = 0; i < plyrCnt; ++i) {
        int home = 0;
        for (int j = 0; j < P_CNT; ++j)
            if (players[i]->getPawn(j).isHome()) home++;
        cout << "Player " << players[i]->getSym() << ":\n";
        cout << " - Pawns Home: " << home << "\n";
        cout << " - Turns Taken: " << turns / MAX_PLAYERS << "\n";
        cout << " - Moves Made: " << players[i]->getMoves() << "\n";
    }
    cout << "Total moves in Game: " << totalMoves.getCount() << "\n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~\n";
}
/*
 * @brief Makes a basic bot move decision and applies it.
 * @param players array of player pointers.
 * @param currIdx index of the bot player.
 * @param card the card number drawn.
 */
void moveBot(PlayerV4* players[], int currIdx, int card) {
    PlayerV4* curr = players[currIdx];

    for (int i = 0; i < P_CNT; i++) {
        PawnV4& p = curr->getPawn(i);
        if (p.isHome()) continue;
        // Try to move out of START
        if (p.getPos() == START && (card == 1 || card == 2)) {
            int newPos = 0;
            // Check if this spot is already occupied by own pawn
            bool clash = false;
            for (int j = 0; j < P_CNT; j++) {
                if (j != i && curr->getPawn(j).getPos() == newPos) {
                    clash = true;
                    break;
                }
            }
            if (!clash) {
                p.setPos(newPos);
                bump(players, currIdx, p.getPos());
                slide(p);
                bump(players, currIdx, p.getPos());
                curr->incrMoves();
                cout << "Bot moved pawn " << i+1 << " to start position 0.\n";
                return;
            }
        }
        // Try to move forward
        if (p.getPos() != START && p.getPos() + card <= B_SIZE) {
            int newPos = p.getPos() + card;
            // Check if this spot is already occupied by own pawn
            bool clash = false;
            for (int j = 0; j < P_CNT; j++) {
                if (j != i && curr->getPawn(j).getPos() == newPos) {
                    clash = true;
                    break;
                }
            }
            if (!clash) {
                if (newPos == B_SIZE) {
                    p.markHome();
                    cout << "Bot moved pawn " << i+1 << " to HOME!\n";
                } else {
                    p.setPos(newPos);
                    bump(players, currIdx, p.getPos());
                    slide(p);
                    bump(players, currIdx, p.getPos());
                    cout << "Bot moved pawn " << i+1 << " to position " << p.getPos() << ".\n";
                }
                curr->incrMoves();
                return;
            }
        }
    }
    cout << "Bot had no valid moves this turn.\n";
}
/*
 * @brief Displays sorted player rankings based on moves made.
 * @param players array of player pointers.
 * @param plyrCnt number of players.
 */
void showSortedStats(PlayerV4* players[], int plyrCnt) {
    // a local array of pointers
    PlayerV4* sorted[MAX_PLAYERS];
    for (int i = 0; i < plyrCnt; ++i)
        sorted[i] = players[i];
    // bubble sort for moves
    for (int i = 0; i < plyrCnt - 1; ++i) {
        for (int j = 0; j < plyrCnt - i - 1; ++j) {
            if (*sorted[j + 1] < *sorted[j]) {
                PlayerV4* temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    // Display results 
    cout << "\n~~~~~ Player Rankings by Least Moves ~~~~~n";
    for (int i = 0; i < plyrCnt; ++i)
        cout << *sorted[i] << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}
/*
 * @brief Overloaded output stream operator for outputting player info.
 * @param out output stream.
 * @param p player object to output.
 * @return Reference to the output stream.
 */
ostream& operator<<(ostream& out, const PlayerV4& p) {
    out << "Player " << p.getSym() << " (" << p.getName() << ") - Moves: " << p.getMoves();
    return out;
}
/*
 * @brief A smarter logic for bot moves based on more strategic logic.
 * @param players array of player pointers.
 * @param idx index of bot player.
 * @param card the card number drawn.
 */
void moveBotHard(PlayerV4* players[], int idx, int card) {
    PlayerV4* curr = players[idx];
    // handle Sorry! card (13)
    if (card == 13) {
        int bestDst = -1, maxDist = -1, tgtPly = -1, tgtPwn = -1;
        // Find opponent pawn furthest from HOME
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (i == idx || players[i] == nullptr) continue;
            for (int j = 0; j < P_CNT; j++) {
                PawnV4& op = players[i]->getPawn(j);
                int pos = op.getPos();
                if (!op.isHome() && pos != START && pos > maxDist) {
                    maxDist = pos;
                    bestDst = pos;
                    tgtPly = i;
                    tgtPwn = j;
                }
            }
        }
        // Check if we have a pawn in START to use
        int ourIdx = -1;
        for (int i = 0; i < P_CNT; i++) {
            if (curr->getPawn(i).getPos() == START) {
                ourIdx = i;
                break;
            }
        }
        if (tgtPly != -1 && ourIdx != -1) {
            players[tgtPly]->getPawn(tgtPwn).sendStart();
            curr->getPawn(ourIdx).setPos(bestDst);
            bump(players, idx, bestDst);
            slide(curr->getPawn(ourIdx));
            bump(players, idx, curr->getPawn(ourIdx).getPos());
            curr->incrMoves();
            cout << "Bot used SORRY! to bump Player " << tgtPly+1 
                 << "'s pawn " << tgtPwn+1 << " and moved to " << bestDst << ".\n";
            return;
        } else {
            cout << "Bot drew SORRY! but no valid move.\n";
            return;
        }
    }
    // Try to move to HOME if possible
    for (int i = 0; i < P_CNT; i++) {
        PawnV4& p = curr->getPawn(i);
        if (!p.isHome() && p.getPos() != START && p.getPos() + card == B_SIZE) {
            p.markHome();
            curr->incrMoves();
            cout << "Bot moved pawn " << i+1 << " to HOME!\n";
            return;
        }
    }
    //Try to move pawn from START if 1 or 2
    for (int i = 0; i < P_CNT; i++) {
        PawnV4& p = curr->getPawn(i);
        if (p.getPos() == START && (card == 1 || card == 2)) {
            // Check for own pawn already at 0
            bool clash = false;
            for (int j = 0; j < P_CNT; j++) {
                if (j != i && curr->getPawn(j).getPos() == 0) {
                    clash = true;
                    break;
                }
            }
            if (clash) continue;
            //if clashes then activate below
            p.setPos(0);
            bump(players, idx, p.getPos());
            slide(p);
            bump(players, idx, p.getPos());
            curr->incrMoves();
            cout << "Bot moved pawn " << i+1 << " to start position 0.\n";
            return;
        }
    }
    // Move furthest pawn
    int farIdx = -1, farPos = -1;
    for (int i = 0; i < P_CNT; i++) {
        int pos = curr->getPawn(i).getPos();
        if (pos != START && !curr->getPawn(i).isHome() && pos > farPos)
            farPos = pos, farIdx = i;
    }
    if (farIdx != -1) {
        PawnV4& p = curr->getPawn(farIdx);
        int newPos = p.getPos() + card;
        if (newPos <= B_SIZE) {
            // Check own-pawn overlap
            bool clash = false;
            for (int j = 0; j < P_CNT; j++) {
                if (j != farIdx && curr->getPawn(j).getPos() == newPos) {
                    clash = true;
                    break;
                }
            }
            //Checks for overlapping
            if (!clash) {
                if (newPos == B_SIZE) {
                    p.markHome();
                    cout << "Bot moved pawn " << farIdx+1 << " to HOME!\n";
                } 
                else {
                    p.setPos(newPos);
                    bump(players, idx, p.getPos());
                    slide(p);
                    bump(players, idx, p.getPos());
                    cout << "Bot moved pawn " << farIdx+1 << " to " << newPos << ".\n";
                }
                curr->incrMoves();
                return;
            }
        }
    }
    cout << "Bot (hard) had no valid moves.\n";
}