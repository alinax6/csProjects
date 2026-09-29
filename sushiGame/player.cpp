/*
 * player.cpp   ss
 * Alina Xie
 * Date: 12/1/2023
 */

#include <iostream>
#include <string>
#include "termfuncs.h"
#include "player.h"
#include "card.h"

using namespace std;

// Player
// Input: no inputs
// Description: default constructor, sets playerNum to 0
// Output: does not return anything
Player::Player() {
    playerNum = 0;
}

// setPlayer
// Input: int corresponding to the player number
// Description: takes in playerNum and sets it playerNum
// Output: does not return anything
void Player::setPlayer(int playerNum) {
    this->playerNum = playerNum;
}

// getPassingHand
// Input: no inputs
// Description: acesses address of vector pointing to passingHands
// Output: returns address of passingHands
Vector* Player::getPassingHand() {
    return &passingHands;
}

// setPassingHand
// Input: vector pointing to hands
// Description: inserts player's selected card to revealCards. Resets 
//              inputCards by removing them and inserts the cards in 
//              player's hand back in revealCards.
// Output: does not return anything
void Player::setPassingHand(Vector* hands) {
    revealCards.push_back(selectedCard);

    while(inputCards.size() > 0) {
        inputCards.pop_back();
    }

    for(int i = 0; i < hands->size(); i++) {
        inputCards.push_back(hands->at(i));
    }
}

// setForDisplay
// Input: has no inputs
// Description: resets passing hand by removing all of the cards and inserts
//              inserts the cards from inputCards to passingHands.
// Output: does not return anything
void Player::setForDisplay() {
    while(passingHands.size() > 0) {
        passingHands.pop_back();
    }

    for(int i = 0; i < inputCards.size(); i++) {
        passingHands.push_back(inputCards.at(i));
    }
}

// getRevealHand
// Input: has no inputs
// Description: acesses the address of vector pointing to revealHand
// Output: returns the address of revealHand
Vector* Player::getRevealHand(){
    return &revealHand;
}

// getRevealedCards
// Input: has no inputs
// Description: acesses the address of vector pointing to revealHand
// Output: returns the address of revealHand
Vector* Player::getRevealedCards() {
    return &revealCards;
}

// getScore
// Input: has no inputs
// Description: checks if it's the end of round and returns the final score
//              or else returns the score at that round
// Output: returns the player's score
int Player::getScore() {
    if(checkEndOfRound) {
        return calculateScore() + makiScore + puddingScore;
    }

    else {
        return totalScore;
    }
}

// getPuddingCount
// Input: has no inputs
// Description: checks if it's the end of round and returns the final pudding
//              or else returns the pudding count at that round
// Output: returns the pudding count
int Player::getPuddingCount() {
    if(checkEndOfRound == true) {
        return revealCardsTypes[SUSHI_TYPES - 1];
    }
    else {
        return totPuddingCount;
    }
}

// setTotPuddingCount
// Input: int corresponding to the pudding count
// Description: takes in totalPuddingCount and sets it count
// Output: does not return anything
void Player::setTotPuddingCount(int count) {
    totPuddingCount = count;
}

// dealCard
// Input: Card pointer 
// Description: inserts card into inputCards and passingHands
// Output: does not return anything
void Player::dealCard(Card* card) {
    inputCards.push_back(card);
    passingHands.push_back(card);
}

// selectCard
// Input: int corresponding to the player's selected card index
// Description: checks if that the cardIndex is valid or else just returns.
//              Resets passingHands and pushes input cards in passingHands.
// Output: does not return anything
void Player::selectCard(int cardIndex) {
    if(cardIndex < 0 || cardIndex >= inputCards.size()) {
        return;
    }

    // Save selected card into selectedCard and will be pushed
    // back to revealedCard later.
    selectedCard = inputCards.at(cardIndex);

    while(passingHands.size() > 0) {
        passingHands.pop_back();
    }

    for(int i = 0; i < inputCards.size(); i++) {
        if(i == cardIndex) {
            continue;
        }
        passingHands.push_back(inputCards.at(i));
    }
}

// reset
// Input: has no inputs
// Description: resets the game for the next round
// Output: does not return anything
void Player::reset() {
    while(revealCards.size() > 0) {
        revealCards.pop_back();
    }
    while(revealHand.size() > 0) {
        revealHand.pop_back();
    }
    selectedCard = nullptr;
    makiScore = 0;
    puddingScore = 0;
    checkEndOfRound = false;
}

// getRevealCardFreq
// Input: has no inputs
// Description: gets the frequency for each type of sushi
// Output: does not return anything
void Player::getRevealCardFreq() {
    wasabiSize = 0;
    nigiriSize = 0;

    for(int j = 0; j < SUSHI_TYPES; j++) {
        revealCardsTypes[j] = 0;
        wasabiPos[j] = 0;
        nigiriPos[j] = 0;
        nigiriUsed[j] = 0;
    }

    // Counts the # of each type of sushi and stores it in revealCardsTypes
    for(int i = 0; i < revealCards.size(); i++) {
        Card* card = revealCards.at(i);
        int index = getIndex(card->getSushiType());

        if(index == 0) revealCardsTypes[index] += card->getMakiCount();
        
        else if(index > 0) {
            revealCardsTypes[index]++;
            string type = card->getSushiType();
            // gets the index when there are wasabi or nigiri cards
            if(type == "Wasabi") {
                wasabiPos[wasabiSize] = i;
                wasabiSize++;
            }
            else if(type == "Squid-Nigiri" || type == "Salmon-Nigiri" ||
                    type == "Egg-Nigiri") {
                nigiriPos[nigiriSize] = i;
                nigiriSize++;
            }
        }
    }
}

// getMakiCount
// Input: has no inputs
// Description: calls getRevealCardFreq and returns the maki count
// Output: returns the maki count
int Player::getMakiCount() {
    getRevealCardFreq();
    return revealCardsTypes[0];
}

// calculateScore
// Input: has no inputs
// Description: calculates the total score by adding together the scores from
//              each type of sushi
// Output: returns the score
int Player::calculateScore() {
    getRevealCardFreq();
    int totalScore = 0;

    // Adds the scores from each type of sushi to totalScore
    for(int i = 1; i < SUSHI_TYPES - 2; i++) {
        if(revealCardsTypes[i] == 0) continue;

        else if(i == 1) totalScore += getTempuraScore(revealCardsTypes[i]);

        else if(i == 2) totalScore += getSashimiScore(revealCardsTypes[i]);

        else if(i == 3) totalScore += getDumplingScore(revealCardsTypes[i]);
    }

    return totalScore + calculateNigiriScore();
}

// setMakiScore
// Input: int corresponding to the maki score
// Description: takes in makiScore and sets it score
// Output: does not return anything
void Player::setMakiScore(int score) {
    makiScore = score;
}

// getIndex
// Input: string corresponding to the sushi type
// Description: assigns the different sushi types to an index
//              from 0-8 and return that index
// Output: returns the index corresponding to the sushi type
int Player::getIndex(string sushiType) {
    if(sushiType == "Maki") return 0;

    else if(sushiType == "Tempura") return 1;

    else if(sushiType == "Sashimi") return 2;
    
    else if(sushiType == "Dumpling") return 3;

    else if(sushiType == "Squid-Nigiri") return 4;

    else if(sushiType == "Salmon-Nigiri") return 5;

    else if(sushiType == "Egg-Nigiri") return 6;

    else if(sushiType == "Wasabi") return 7;

    else if(sushiType == "Pudding") return 8;

    else return -1;
}

// getTempuraScore
// Input: int corresponding to the number of tempura cards
// Description: calculates the tempura score and returns it
// Output: returns the tempura score
int Player::getTempuraScore(int numCards) {
    int pairs = numCards / 2;

    return pairs * 5;
}

// getSashimiScore
// Input: int corresponding to the number of sashimi cards
// Description: calculates the sashimi score and returns it
// Output: returns the sashimi score
int Player::getSashimiScore(int numCards) {
    int triples = numCards / 3;

    return triples * 10;
}

// getDumplingScore
// Input: int corresponding to the number of dumpling cards
// Description: calculates the dumpling score and returns it
// Output: returns the dumpling score
int Player::getDumplingScore(int numCards) {
    if(numCards >= 5) {
        return 15;
    }
    else if(numCards == 4) {
        return 10;
    }
    else if(numCards == 3) {
        return 6;
    }
    else if(numCards == 2) {
        return 3;
    }
    else {
        return 1;
    }
}

// calculateNigiriScore
// Input: has no inputs
// Description: checks if wasabi is before the nigiri and calculates
//              the nigiri score plus wasabi score if applicable
// Output: returns the total nigiri plus wasabi score
int Player::calculateNigiriScore() {
    int total = 0;
    for(int i = wasabiSize - 1; i >= 0; i--) {
        int selectedScore = -1;
        int index = -1;
        for(int j = nigiriSize - 1; j >= 0; j--) {
            // checks if wasabi card is before a nigiri card
            if(wasabiPos[i] > nigiriPos[j]) continue;
            if(nigiriUsed[j] == 1) continue;

            // finds the nigiri closest to the wasabi
            int nScore = getNigiriCardScore(nigiriPos[j]);
            if(index == -1 || nigiriPos[index] > nigiriPos[j]) {
                index = j;
                selectedScore = nScore;
            }
        }
        // calculates the wasabi score and sets nigiri as used (1)
        if(index == -1) continue;
        total += selectedScore * 3;
        nigiriUsed[index] = 1;  
    }

    //if nigiri hasn't been used & adds nigiri score to wasabi score
    for(int i = 0; i < nigiriSize; i++) {
        if(nigiriUsed[i] == 1) continue;
        int nScore = getNigiriCardScore(nigiriPos[i]);
        total += nScore;
    }
    return total;
}

// getNigiriCardScore
// Input: int corresponding to index of the nigiri card
// Description: returns 3, 2, 1 depending if the nigiri card is
//              squid, salmon, or egg nigiri
// Output: returns the number corresponding the different types
//         of nigiri
int Player::getNigiriCardScore(int cardIndex) {
    Card* card = revealCards.at(cardIndex);
    string type = card->getSushiType();

    if(type == "Squid-Nigiri") {
        return 3;
    }
    if(type == "Salmon-Nigiri") {
        return 2;
    }
    if(type == "Egg-Nigiri") {
        return 1;
    }

    return 0;
}

// setPuddingScore
// Input: int corresponding to the pudding score
// Description: takes in puddingScore and sets it score
// Output: does not return anything
void Player::setPuddingScore(int score) {
    puddingScore = score;
}

// setEndOfRound
// Input: bool corresponding to if the end of the round is reached
// Description: takes in checkEndOfRound and sets it check
// Output: does not return anything
void Player::setEndOfRound(bool check) {
    checkEndOfRound = check;
}

// setPlayerTotScore
// Input: int corresponding to the final total score
// Description: takes in totalScore and sets it score
// Output: does not return anything
void Player::setPlayerTotScore(int score) {
    totalScore = score;
}






