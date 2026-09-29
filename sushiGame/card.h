/*
 * card.h
 * Alina Xie
 * Date: 12/1/2023
 */

#ifndef CARD_H
#define CARD_H

#include <iostream>
#include <string>
#include "termfuncs.h"


using namespace std; 

class Card {
    public:
    Card(string sushiType = "", int makiCount = 0);

    // getters
    string getSushiType();
    int getMakiCount();

    private:
    string sushiType;
    int makiCount;
};

#endif

