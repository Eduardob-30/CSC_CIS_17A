/* 
 * File:   SorryProject2V2.cpp
 * Author: eddiebrito
 *
 * Created on May 24, 2025, 12:05 PM
 */
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include "Game_Classes_V2.h"
using namespace std;

// Function Prototypes
int draw();
void move(PlayerV2[], int, int);
void slide(PawnV2 &);
void bump(PlayerV2[], int, int);
bool isDone(PlayerV2);
void showBoard(PlayerV2[], int);
void saveGame(PlayerV2[], int, string);
bool loadGame(PlayerV2[], int &, string);
void showStats(PlayerV2[], int);

// Main Program
int main() {
    srand(static_cast<unsigned int>(time(0)));
    const int B_SIZE = 60;
    PlayerV2 players[2];
    int turn = 0;
    char choice;

    cout << "Load saved game? (y/n): ";
    cin >> choice;
    cin.ignore();

    if (choice == 'y' || choice == 'Y') {
        string fileName;
        cout << "Enter save file name to load (with .bin): ";
        getline(cin, fileName);
        if (!loadGame(players, turn, fileName)) {
            cout << "Failed to load. Starting new game.\n";
            for (int i = 0; i < 2; i++) {
                players[i].init(i == 0 ? 'A' : 'B');
            }
        }
    } else {
        for (int i = 0; i < 2; i++) {
            players[i].init(i == 0 ? 'A' : 'B');
        }
    }

    while (true) {
        int curr = turn % 2;
        cout << "\nTurn: " << players[curr].getName() << " (" << players[curr].getSym() << ")\n";
        showBoard(players, B_SIZE);
        int card = draw();
        cout << "Card drawn: " << card << "\n";
        move(players, curr, card);

        if (isDone(players[curr])) {
            cout << players[curr].getName() << " wins!\n";
            showStats(players, turn + 1);
            break;
        }

        char cont;
        cout << "\n[s] Save and quit\n[q] Quit without saving\n[c] Continue\nEnter choice: ";
        cin >> cont;
        cin.ignore();
        if (cont == 's') {
            string fn;
            cout << "Enter filename (no extension): ";
            getline(cin, fn);
            saveGame(players, turn, fn + ".bin");
            break;
        } else if (cont == 'q') break;

        turn++;
    }

    return 0;
}

// Function Definitions
int draw() {
    return rand() % 12 + 1;
}

void move(PlayerV2 players[], int curr, int card) {
    PlayerV2 &plyr = players[curr];
    bool moved = false;
    int choice;

    for (int i = 0; i < 2; i++) {
        if (plyr.getPawn(i).isHome()) continue;
        if (plyr.getPawn(i).getPos() == -1 && (card == 1 || card == 2)) {
            plyr.getPawn(i).setPos(0);
            cout << "Pawn " << i + 1 << " leaves START.\n";
            slide(plyr.getPawn(i));
            bump(players, curr, plyr.getPawn(i).getPos());
            moved = true;
            break;
        } else if (plyr.getPawn(i).getPos() != -1 && plyr.getPawn(i).getPos() + card <= 60) {
            plyr.getPawn(i).setPos(plyr.getPawn(i).getPos() + card);
            cout << "Pawn " << i + 1 << " moved forward.\n";
            slide(plyr.getPawn(i));
            bump(players, curr, plyr.getPawn(i).getPos());
            if (plyr.getPawn(i).getPos() == 60) plyr.getPawn(i).setHome(true);
            moved = true;
            break;
        }
    }

    if (!moved) cout << "No valid moves.\n";
    else plyr.addMove();
}

void slide(PawnV2 &p) {
    const int slides[] = {5, 15, 25, 35, 45, 55};
    for (int i = 0; i < 6; i++) {
        if (p.getPos() == slides[i]) {
            p.setPos(p.getPos() + 3);
            cout << "Landed on slide! Now at: " << p.getPos() << "\n";
        }
    }
}

void bump(PlayerV2 players[], int curr, int pos) {
    for (int i = 0; i < 2; i++) {
        if (i == curr) continue;
        for (int j = 0; j < 2; j++) {
            if (players[i].getPawn(j).getPos() == pos && !players[i].getPawn(j).isHome()) {
                players[i].getPawn(j).setPos(-1);
                cout << "Bumped Player " << players[i].getSym() << "'s Pawn " << j + 1 << "!\n";
            }
        }
    }
}

bool isDone(PlayerV2 plyr) {
    return plyr.getPawn(0).isHome() && plyr.getPawn(1).isHome();
}

void showBoard(PlayerV2 players[], int size) {
    for (int i = 0; i < size; i++) {
        bool placed = false;
        for (int p = 0; p < 2; p++) {
            for (int j = 0; j < 2; j++) {
                if (players[p].getPawn(j).getPos() == i) {
                    cout << "[" << players[p].getSym() << j + 1 << "]";
                    placed = true;
                }
            }
        }
        if (!placed) cout << "[--]";
        if ((i + 1) % 10 == 0) cout << endl;
    }
}

void saveGame(PlayerV2 players[], int turn, string fileName) {
    ofstream out(fileName.c_str(), ios::binary);
    if (out) {
        out.write(reinterpret_cast<char*>(&turn), sizeof(int));
        out.write(reinterpret_cast<char*>(&players[0]), sizeof(PlayerV2));
        out.write(reinterpret_cast<char*>(&players[1]), sizeof(PlayerV2));
        out.close();
    }
}

bool loadGame(PlayerV2 players[], int &turn, string fileName) {
    ifstream in(fileName.c_str(), ios::binary);
    if (in) {
        in.read(reinterpret_cast<char*>(&turn), sizeof(int));
        in.read(reinterpret_cast<char*>(&players[0]), sizeof(PlayerV2));
        in.read(reinterpret_cast<char*>(&players[1]), sizeof(PlayerV2));
        in.close();
        return true;
    }
    return false;
}

void showStats(PlayerV2 players[], int turns) {
    cout << "\n--- Game Summary ---\n";
    for (int i = 0; i < 2; i++) {
        int homeCount = 0;
        for (int j = 0; j < 2; j++)
            if (players[i].getPawn(j).isHome()) homeCount++;

        cout << "Player " << players[i].getSym() << ":\n";
        cout << " - Pawns Home: " << homeCount << "\n";
        cout << " - Moves: " << players[i].getMoves() << "\n";
        cout << " - Turns: " << turns / 2 << "\n";
    }
}