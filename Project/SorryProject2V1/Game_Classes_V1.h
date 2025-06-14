/* 
 * File:   Game_Classes.h
 * Author: eddiebrito
 *
 */
#ifndef GAME_CLASSES_V1_H
#define GAME_CLASSES_V1_H

#include <iostream>
#include <string>
using namespace std;

// Global Constants
const int B_SIZE = 60;
const int P_CNT = 2;
const int SLIDE_LEN = 3;
const int SLIDE_COUNT = 6;
const int SLIDE_STARTS[SLIDE_COUNT] = {5, 15, 25, 35, 45, 55};

// Enumerators for pawn positions
enum PosStat { START = -1, HOME = -2 };

// Class to represent a single pawn
class PawnV1 {
private:
    int pos;
    bool home;

public:
    PawnV1() : pos(START), home(false) {}

    int getPos() const {
        return pos;
    }

    bool isHome() const {
        return home;
    }

    void setPos(int p) {
        pos = p;
    }

    void sendHome() {
        pos = HOME;
        home = true;
    }

    void sendStart() {
        pos = START;
        home = false;
    }

    void markHome() {
        home = true;
        pos = HOME;
    }

    void setHome(bool h) {
        home = h;
    }
};

// Class to represent a player with multiple pawns
class PlayerV1 {
private:
    string name;
    char sym;
    int moves;
    PawnV1 pawns[P_CNT];

public:
    PlayerV1() : name(""), sym(' '), moves(0) {}

    void setName(string n) {
        name = n;
    }

    string getName() const {
        return name;
    }

    void setSym(char s) {
        sym = s;
    }

    char getSym() const {
        return sym;
    }

    int getMoves() const {
        return moves;
    }

    void incrMoves() {
        moves++;
    }

    PawnV1& getPawn(int i) {
        return pawns[i];
    }

    void reset() {
        moves = 0;
        for (int i = 0; i < P_CNT; ++i) {
            pawns[i].sendStart();
        }
    }

    bool allHome() const {
        for (int i = 0; i < P_CNT; ++i)
            if (!pawns[i].isHome()) return false;
        return true;
    }
};

#endif // GAME_CLASSES_V1_H