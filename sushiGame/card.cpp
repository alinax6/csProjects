/*
 * card.cpp
 * Alina Xie
 * Date: 12/1/2023
 */

#include <iostream>
#include <string>
#include "termfuncs.h"
#include "card.h"

using namespace std;

Card::Card(string sushiType, int makiCount) {
    this->sushiType = sushiType;
    this->makiCount = makiCount;

}


// getSushiType
// Input: no inputs
// Description: accesses the sushiType string
// Output: returns the sushiType string
string Card::getSushiType() {
   return sushiType;
}

// getMakiCount
// Input: no inputs
// Description: accesses the makiCount int
// Output: returns the makiCount int
int Card::getMakiCount() {
    return makiCount;
}
