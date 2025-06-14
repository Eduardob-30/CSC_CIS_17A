/* 
 * File:   SorryProject2V1.cpp
 * Author: eddiebrito
 *
 * Created on May 23, 2025, 11:53 AM
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "Game_Classes_V1.h"
using namespace std;

// Function Prototypes
int draw();
void move(PlayerV1 players[], int currIdx, int card);
bool isDone(const PlayerV1&);
void showBoard(PlayerV1[], int);

int main() {
    srand(static_cast<unsigned int>(time(0)));

    PlayerV1 players[2];
    for (int i = 0; i < 2; i++) {
        string pname;
        cout << "Enter name for Player " << i + 1 << ": ";
        getline(cin, pname);
        players[i].setName(pname);
        players[i].setSym(i == 0 ? 'A' : 'B');
    }

    int turn = 0;
    while (true) {
        int currIdx = turn % 2;
        PlayerV1& curr = players[currIdx];
        cout << "\nTurn: " << curr.getName() << " (" << curr.getSym() << ")\n";
        showBoard(players, B_SIZE);
        int card = draw();
        cout << "Card drawn: " << card << "\n";
        move(players, currIdx, card);
        if (isDone(curr)) {
            cout << curr.getName() << " wins! All pawns are HOME!\n";
            break;
        }
        turn++;
    }
    return 0;
}

// Draw random card (1–12)
int draw() {
    return rand() % 12 + 1;
}

// Move logic
void move(PlayerV1 players[], int currIdx, int card) {
    PlayerV1& curr = players[currIdx];
    int attempts = 0;
    bool moved = false;

    while (!moved && attempts < P_CNT) {
        int ch;
        cout << "Choose pawn to move (1 or 2): ";
        cin >> ch;
        ch--;
        if (ch < 0 || ch >= P_CNT) {
            cout << "Invalid pawn.\n";
            continue;
        }

        PawnV1& p = curr.getPawn(ch);
        if (p.isHome()) {
            cout << "That pawn is already HOME.\n";
        } else if (p.getPos() == START && (card == 1 || card == 2)) {
            p.setPos(0);
            cout << "Moved pawn " << ch + 1 << " from START to 0.\n";
            moved = true;
        } else if (p.getPos() == START) {
            cout << "Need a 1 or 2 to leave START.\n";
        } else if (p.getPos() + card > B_SIZE) {
            cout << "Too far to move.\n";
        } else if (p.getPos() + card == B_SIZE) {
            p.sendHome();
            cout << "Pawn " << ch + 1 << " reached HOME!\n";
            moved = true;
        } else {
            p.setPos(p.getPos() + card);
            cout << "Pawn " << ch + 1 << " moved to " << p.getPos() << ".\n";
            moved = true;
        }

        if (moved) curr.incrMoves();
        attempts++;
    }

    if (!moved) {
        cout << "No valid moves.\n";
    }
}

// Check if all pawns are home
bool isDone(const PlayerV1& p) {
    return p.allHome();
}

// Display game board 
void showBoard(PlayerV1 players[], int size) {
    for (int row = 0; row < 6; row++) {
        for (int i = 0; i < 10; i++) {
            int idx = row * 10 + i;
            bool found = false;
            for (int p = 0; p < 2; p++) {
                for (int j = 0; j < P_CNT; j++) {
                    if (players[p].getPawn(j).getPos() == idx) {
                        cout << "[" << players[p].getSym() << j + 1 << "]";
                        found = true;
                    }
                }
            }
            if (!found) cout << "[--]";
        }
        cout << endl;
    }
}