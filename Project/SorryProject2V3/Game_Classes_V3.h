/* 
 * File:   Game_Classes_V3.h
 * Author: eddiebrito
 *
 */
// Game_Classes_V3.h
#ifndef GAME_CLASSES_V3_H
#define GAME_CLASSES_V3_H

#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

// Global Constants
const int B_SIZE = 60;
const int P_CNT = 2;
const int SLIDE_LEN = 3;
const int SLIDE_COUNT = 6;
const int SLIDE_STARTS[SLIDE_COUNT] = {5, 15, 25, 35, 45, 55};
const int MAX_PLAYERS = 4;

// Enumerators for pawn positions
enum PosStat { START = -1, HOME = -2 };

/**
 * @class PawnV3
 * @brief Represents a pawn in the game, tracking its state.
 */
class PawnV3 {
private:
    int pos;
    bool home;
    int prevPos;
    bool washome;

public:
    PawnV3() : pos(START), home(false), prevPos(START), washome(false) {}

    int getPos() const { return pos; }
    bool isHome() const { return home; }

    void setPos(int p) { pos = p; }
    void sendHome() { pos = HOME; home = true; }
    void sendStart() { pos = START; home = false; }
    void markHome() { home = true; pos = HOME; }

    void backup() { prevPos = pos; washome = home; }
    void restore() { pos = prevPos; home = washome; }
};

/**
 * @class PlayerV3
 * @brief Represents a human or bot player.
 */
class PlayerV3 {
protected:
    string name;
    char sym;
    PawnV3 pawns[P_CNT];
    int moves;
    static int plyrCnt;

public:
    PlayerV3() : name(""), sym(' '), moves(0) { plyrCnt++; }

    virtual void setName(string n) { name = n; }
    virtual string getName() const { return name; }
    char getSym() const { return sym; }
    int getMoves() const { return moves; }
    void setSym(char s) { sym = s; }
    void incrMoves() { moves++; }

    PawnV3& getPawn(int i) { return pawns[i]; }

    bool allHome() const {
        for (int i = 0; i < P_CNT; ++i)
            if (!pawns[i].isHome()) return false;
        return true;
    }

    void reset() {
        moves = 0;
        for (int i = 0; i < P_CNT; ++i)
            pawns[i].sendStart();
    }

    virtual bool isBot() const { return false; }

    static int getPlyrCnt() { return plyrCnt; }

    bool operator<(const PlayerV3& other) const {
        return moves < other.moves;
    }
};

/**
 * @class Bot
 * @brief Bot player derived from PlayerV3.
 */
class Bot : public PlayerV3 {
private:
    bool isHard;

public:
    Bot(bool hard = false) : isHard(hard) {
        name = hard ? "HardBot" : "Bot";
    }

    bool isBot() const override { return true; }
    bool hardMode() const { return isHard; }

    string getName() const override { return name; }
    void setName(string n) override { name = n; }
};

/**
 * @class CardDeck
 * @brief Simulates a card deck for the game.
 */
class CardDeck {
private:
    int cards[45];
    int idx;

public:
    CardDeck() { shuffle(); }

    void shuffle() {
        idx = 0;
        for (int i = 0; i < 45; ++i)
            cards[i] = (i % 13) + 1;

        for (int i = 0; i < 45; ++i) {
            int j = rand() % 45;
            int tmp = cards[i];
            cards[i] = cards[j];
            cards[j] = tmp;
        }
    }

    int draw() {
        if (idx >= 45) shuffle();
        return cards[idx++];
    }
};

ostream& operator<<(ostream& out, const PlayerV3& p);

#endif // GAME_CLASSES_V3_H