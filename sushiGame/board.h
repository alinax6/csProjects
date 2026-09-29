/*
 * board.h
 * Alina Xie
 * Date: 12/1/2023
 */

#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include "termfuncs.h"
#include "player.h"
#include "card.h"
#include "vector.h"


using namespace std;

class Board{ 
    public:
        Board();

        //drawing functions
        void drawBoard(Player *players, int turn);
        void drawScore(Player *player);
        void drawWinner(Player *players, int winner);

    private:
        //drawing helper functions
        void drawRound(int p_index, Player *player);
        void drawLabel(int vpos, int hpos, string label);
        void drawPlayer(int p_index, Player *p, int turn);
        void drawCard(int vpos, int hpos, int index, Card *c, bool unplayed);
        void drawPudding(int vpos, int hpos);
        void drawWasabi(int vpos, int hpos);
        void drawDumpling(int vpos, int hpos);
        void drawSashimi(int vpos, int hpos);
        void drawTempura(int vpos, int hpos);
        void drawMaki(int vpos, int hpos, int count);
        void drawChopsticks(int vpos, int hpos);
        void drawNigiri(int vpos, int hpos, string type, bool wasabi);
        void drawBlank(int vpos, int hpos);
        string formatScore(int score);
        void clearBoard();

        //constants
        static const int PLAYER_COUNT = 3;
        static const int BOARD_HEIGHT = 42;
        static const int BOARD_WIDTH = 120;
        static const int CARD_WIDTH = 13;
        static const int CARD_HEIGHT = 8;
        static const int CARD_ROWS = 3;
        static const int CARD_COLS = 4;
        static const int MAX_CARDS = 9;

        //display
        char board[BOARD_HEIGHT][BOARD_WIDTH];
        string board_color[BOARD_HEIGHT][BOARD_WIDTH];

};

#endif
