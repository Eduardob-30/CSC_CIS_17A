/* 
 * File:   Game_Classes.h
 * Author: eddiebrito
 *
 * Created on May 30, 2025, 1:59 PM
 */
// GameClasses.h
#ifndef GAME_CLASSES_H
#define GAME_CLASSES_H

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

using namespace std;
/**
 * @class PawnV4
 * @brief Represents a single pawn in the Sorry! game, tracking position and home status.
 */
class PawnV4 {
private:
    int pos;        //position of pawn
    bool washome;   //flag for if pawn home
    int prevPos;    // previous position
    bool home;      // if pawn is home

public:
    PawnV4() : pos(START), prevPos(START), home(false), washome(false) {}
    //pawn current position
    int getPos() const {
        return pos;        
    }
    //if pawn is home
    bool isHome() const {
        return home;        
    }
    //set pawns position
    void setPos(int p) {
        pos = p;            
    }
     //sends pawn home
    void sendHome() {
        pos = HOME;
        home = true;       
    } 
    //sends pawn to start and clears home flag
    void sendStart() {
        pos = START;
        home = false;       
    }
    //marks if pawn is home
    void markHome() {
        home = true;
        pos = HOME;         
    }
    //saves position
    void backup() {
        prevPos = pos;
        washome = home;     
    }
     //restores last spot
    void restore() {
        pos = prevPos;
        home = washome;    
    }
};
/*
 * @class PlayerV4
 * @brief Manages moves of pawns for players/bot
 */
class PlayerV4 {
protected:
    string name;
    char sym;   //symbol for board
    PawnV4 pawns[P_CNT];    //array for player pawns
    int moves;  //# moves made
    static int plyrCnt; 

public:
    PlayerV4() : name(""), sym(' '), moves(0) {
    plyrCnt++;
    }

    virtual void setName(string n) { 
        name = n; 
    }
    virtual string getName() const {
       return name; 
    }
    char getSym() const { 
        return sym; 
    }
    int getMoves() const { 
        return moves; 
    }
    //compare players moves
    bool operator<(const PlayerV4& other) const {
        return moves < other.moves;
    }
    void setSym(char s) { 
        sym = s; 
    }
    //increment move counter
    void incrMoves() { 
        moves++; 
    }
    PawnV4& getPawn(int i) { 
        return pawns[i]; 
    }
    //checks if all pawns are home
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
    //check if there is a bot when there is 1 player
    virtual bool isBot() const { 
        return false; 
    }
    static int getPlyrCnt() {
        return plyrCnt;
    }
};
/*
 * @class Bot
 * @brief Represents a bot player, either basic or hard difficulty.
 * Inherits from PlayerV4.
 */
class Bot : public PlayerV4 {
private:
    bool isHard;  
    // true is hard bot
public:
    /*
     * @brief Constructor to set bot difficulty.
     * @param hard true for hard bot, false for basic bot.
     */
    Bot(bool hard = false) : isHard(hard) {
        name = hard ? "HardBot" : "Bot";
    }
    //always a bot
    bool isBot() const override { 
        return true; 
    }
    //if selected hard bot
    bool hardMode() const {
        return isHard;
    }
    string getName() const override {
        return name;
    }

    void setName(string n) override {
        name = n;
    }
};
/*
 * @class CardDeck
 * @brief Shows a shuffled deck of Sorry! cards.
 */
class CardDeck {
private:
    int cards[45];
    int idx;

public:
    CardDeck() { 
        shuffle(); 
    }
    /**
     * @brief Shuffles the card deck randomly.
     */
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
    /*
     * @brief Draws the next card from the deck.
     * reshuffles when deck is empty.
     * @return Drawn card number (1–13).
     */
    int draw() {
        if (idx >= 45) shuffle();
        return cards[idx++];
    }
};
ostream& operator<<(ostream& out, const PlayerV4& p);

#endif // GAME_CLASSES_H