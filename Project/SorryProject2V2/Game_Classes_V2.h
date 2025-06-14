/* 
 * File:   Game_Classes_V2.h
 * Author: eddiebrito
 *
 */
#ifndef GAME_CLASSES_V2_H
#define GAME_CLASSES_V2_H

#include <string>
using namespace std;

// Constants used across classes
const int START = -1;
const int HOME = -2;
const int P_CNT = 2;

class PawnV2 {
private:
    int pos;
    bool home;
public:
    PawnV2() {
        pos = START;
        home = false;
    }

    void setPos(int p) { pos = p; }
    int getPos() const { return pos; }

    void setHome(bool h) { home = h; }
    bool isHome() const { return home; }
};

class PlayerV2 {
private:
    string name;
    char sym;
    int moves;
    PawnV2 pawns[P_CNT];
public:
    PlayerV2() {
        name = "";
        sym = ' ';
        moves = 0;
    }

    void init(char s) {
        cout << "Enter name for Player " << s << ": ";
        getline(cin, name);
        sym = s;
        moves = 0;
        for (int i = 0; i < P_CNT; i++) {
            pawns[i] = PawnV2();
        }
    }

    string getName() const { return name; }
    char getSym() const { return sym; }
    int getMoves() const { return moves; }

    void addMove() { moves++; }

    PawnV2& getPawn(int i) { return pawns[i]; }
};

#endif