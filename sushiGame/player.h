/*
 * player.h
 * Alina Xie
 * Date: 12/1/2023
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include "termfuncs.h"
#include "vector.h"
#include "card.h"

using namespace std;

class Player{
    public:
    Player();
    static const int SUSHI_TYPES = 9;

    // setters and getters
    void setPlayer(int playerNum);
    Vector* getPassingHand();
    Vector* getRevealHand();
    Vector* getRevealedCards();
    int getScore();
    int getPuddingCount();
    void setPassingHand(Vector* hands);
    void setForDisplay();

    // methods
    void dealCard(Card* card);
    void selectCard(int cardIndex);
    void reset();
    void getRevealCardFreq();
    int getMakiCount();
    int calculateScore();
    void setMakiScore(int score);
    int getIndex(string sushiType);
    int getTempuraScore(int numCards);
    int getSashimiScore(int numCards);
    int getDumplingScore(int numCards);
    void setPuddingScore(int score);
    void setEndOfRound(bool check = true);
    void setPlayerTotScore(int score);
    void setTotPuddingCount(int count);
    int getNigiriCardScore(int cardIndex);
    int calculateNigiriScore();

    // Player objects
    public:
    int playerNum;

    private:
    Vector inputCards;
    Vector passingHands;
    Vector revealCards;
    Vector revealHand;
    Card* selectedCard = nullptr;
    int revealCardsTypes[SUSHI_TYPES];
    int makiScore = 0;
    bool checkEndOfRound = false;
    int puddingScore = 0;
    int totalScore = 0;
    int totPuddingCount = 0;

    // Calculate nigiri and wasabi score
    int wasabiPos[SUSHI_TYPES];
    int nigiriPos[SUSHI_TYPES];
    int nigiriUsed[SUSHI_TYPES];
    int wasabiSize = 0;
    int nigiriSize = 0;
};

#endif