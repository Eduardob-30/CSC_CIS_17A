/* 
 * File:   SorryProject2V3.cpp
 * Author: eddiebrito
 *
 * Created on May 26, 2025, 4:29 PM
 */
// System Libraries
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include "Game_Classes_V3.h"
using namespace std;

// Static member initialization
int PlayerV3::plyrCnt = 0;

// Function Prototypes
// Game logic
int draw(CardDeck &);
void move(PlayerV3*[], int, int);
void moveBot(PlayerV3*[], int, int);
void moveBotHard(PlayerV3*[], int, int);
void slide(PawnV3 &);
void bump(PlayerV3*[], int, int);
bool isDone(PlayerV3*);
void undoMove(PlayerV3*[], int);
// Outputs
void showBoard(PlayerV3*[], int);
void showStats(PlayerV3*[], int, int);
void showSortedStats(PlayerV3*[], int); \
// File I/O
void saveGame(PlayerV3*[], int, string);
bool loadGame(PlayerV3*[], int &, string);
int main() {
    srand(static_cast<unsigned int>(time(0)));

    PlayerV3* players[MAX_PLAYERS] = {nullptr};
    int plyrCnt = 2;
    int turn = 0;
    char choice;
    bool hardBot = false;  // bot difficulty
    int totalMoves = 0;    

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
                cout << "Enter save file name to load (with .bin): ";
                string fileName;
                getline(cin, fileName);
                bool loaded = loadGame(players, turn, fileName);
                if (!loaded) {
                    cout << "Save file not found. Starting new game.\n";
                    menuChoice = 3;
                } else {
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
    } while (menuChoice != 3 && menuChoice != 0);

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
            players[0] = new PlayerV3();
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
            } while (diff != 1 && diff != 2);

            players[1] = new Bot();  // Inherits from PlayerV3
            players[1]->setSym('B');
            plyrCnt = 2;
        } else {
            for (int i = 0; i < plyrCnt; i++) {
                string name;
                cout << "Enter name for Player " << i + 1 << ": ";
                getline(cin, name);
                players[i] = new PlayerV3();
                players[i]->setName(name);
                players[i]->setSym('A' + i);
            }
        }
    }

    CardDeck deck;
    while (true) {
        int currIdx = turn % plyrCnt;
        PlayerV3* curr = players[currIdx];
        cout << "\nTurn: " << curr->getName() << " (" << curr->getSym() << ")\n";
        showBoard(players, B_SIZE);
        int card = draw(deck);
        cout << "Card drawn: " << card << "\n";

        if (curr->isBot()) {
            if (hardBot) moveBotHard(players, currIdx, card);
            else moveBot(players, currIdx, card);
        } else {
            move(players, currIdx, card, totalMoves);
        }

        if (isDone(players[currIdx])) {
            cout << "\n" << curr->getName() << " wins! All pawns are HOME!\n";
            showStats(players, turn + 1, plyrCnt, totalMoves);
            showSortedStats(players, plyrCnt);
            break;
        }

        char cont;
        cout << "\nOptions:\n";
        cout << "[s] Save and quit\n";
        cout << "[u] Undo last move\n";
        cout << "[q] Quit without saving\n";
        cout << "[c] Continue playing\n";
        cout << "Enter your choice: ";
        cin >> cont;
        cin.ignore();

        if (cont == 's' || cont == 'S') {
            string fileName;
            cout << "Enter save file name (no extension): ";
            getline(cin, fileName);
            saveGame(players, turn, fileName + ".bin");
            cout << "Game saved.\n";
            break;
        } else if (cont == 'q' || cont == 'Q') {
            cout << "Exiting without saving...\n";
            break;
        } else if (cont == 'u' || cont == 'U') {
            undoMove(players, currIdx);
            if (curr->isBot()) {
                if (hardBot) moveBotHard(players, currIdx, card);
                else moveBot(players, currIdx, card);
            } else {
                move(players, currIdx, card, totalMoves);
            }
        }

        turn++;
    }

    for (int i = 0; i < MAX_PLAYERS; ++i)
        delete players[i];

    return 0;
}
int draw(CardDeck &deck) {
    return deck.draw();
}

void slide(PawnV3 &p) {
    for (int i = 0; i < SLIDE_COUNT; i++) {
        if (p.getPos() == SLIDE_STARTS[i]) {
            p.setPos(p.getPos() + SLIDE_LEN);
            cout << "Landed on slide! New position: " << p.getPos() << "\n";
        }
    }
}

void bump(PlayerV3* players[], int currIdx, int pos) {
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

bool isDone(PlayerV3* p) {
    return p->allHome();
}

void showBoard(PlayerV3* players[], int size) {
    for (int row = 0; row < 6; row++) {
        for (int i = 0; i < 10; i++) {
            int idx = row * 10 + i;
            bool found = false;
            for (int p = 0; p < MAX_PLAYERS; p++) {
                if (players[p] != nullptr) {
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

void saveGame(PlayerV3* players[], int turn, string fileName) {
    ofstream out(fileName.c_str(), ios::binary);
    if (!out) {
        cerr << "Could not open file for saving.\n";
        return;
    }
    out.write(reinterpret_cast<char*>(&turn), sizeof(int));
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        if (players[i]) {
            out.write(reinterpret_cast<char*>(players[i]), sizeof(PlayerV3));
        } else {
            PlayerV3 dummy;
            out.write(reinterpret_cast<char*>(&dummy), sizeof(PlayerV3));
        }
    }
    out.close();
    cout << "Game saved successfully to " << fileName << "\n";
}

bool loadGame(PlayerV3* players[], int &turn, string fileName) {
    ifstream in(fileName.c_str(), ios::binary);
    if (!in) {
        cout << "Could not open file for reading.\n";
        return false;
    }
    in.read(reinterpret_cast<char*>(&turn), sizeof(int));
    in.read(reinterpret_cast<char*>(players), sizeof(PlayerV3) * MAX_PLAYERS);
    in.close();
    return true;
}

void undoMove(PlayerV3* players[], int currIdx) {
    PlayerV3* curr = players[currIdx];
    for (int i = 0; i < P_CNT; i++) {
        curr->getPawn(i).restore();
    }
    cout << "Undo complete: Restored previous positions of your pawns.\n";
}

void showStats(PlayerV3* players[], int turns, int plyrCnt) {
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
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~\n";
}

void moveBot(PlayerV3* players[], int currIdx, int card) {
    PlayerV3* curr = players[currIdx];

    for (int i = 0; i < P_CNT; i++) {
        PawnV3& p = curr->getPawn(i);
        if (p.isHome()) continue;
        if (p.getPos() == START && (card == 1 || card == 2)) {
            int newPos = 0;
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
        if (p.getPos() != START && p.getPos() + card <= B_SIZE) {
            int newPos = p.getPos() + card;
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