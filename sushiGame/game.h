/*
 * game.h
 * Alina Xie
 * Date: 12/1/2023
 */

#ifndef GAME_H
#define GAME_H
#include <iostream>
#include <fstream>
#include <string>
#include "termfuncs.h"
#include "board.h"
#include "vector.h"
#include "player.h"
#include "card.h"

using namespace std;

class Game{
    public:
        //constructor/destructor
        Game(string filename, string play_chopsticks);
        ~Game();

        //main gameplay
        void playGame();

    private:
        //constants
        static const int PLAYER_COUNT = 3;
        static const int ROUNDS = 3;
        static const int CARD_HAND = 9;

        //chopsticks activated true/false
        bool play_chopsticks;

        //card deck
        Vector deck;

        //game objects
        Board board;
        Player players[PLAYER_COUNT];
        int playerTotalScore[PLAYER_COUNT];
        int playerTotalPudding[PLAYER_COUNT];

        // methods
        void makeDeck(string filename);
        bool dealCards(int* startPoint);
        int* sortCounts(int* ogCounts);
        int* sortedPlayerNums(int* sortedCount, int* ogCount);
        int* makiScoring(int* makiCounts, int* playerNums);
        int* puddingScoring(int* puddingCounts, int* playerNums);
        void checkMaki();
        void checkPudding();
         
};

#endif
