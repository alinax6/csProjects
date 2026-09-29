/*
 * game.cpp
 * Alina Xie
 * Date: 12/1/2023
 */

#include "game.h"
#include "vector.h"
#include "card.h"
#include "player.h"


//constructor
Game::Game(string filename, string play_chopsticks){
    //include chopsticks or not
    if(play_chopsticks == "true"){
        this->play_chopsticks = true;
    }else{
        this->play_chopsticks = false;
    }  

    /*-----------------------------------------------*/
    /*   TODO (Part I): FINISH SETTING UP THE GAME   */
    /*                                               */
    /*   Read from the input file to initialize the  */
    /*   deck.                                       */
    /*-----------------------------------------------*/ 
    makeDeck(filename); 
}

// makeDeck
// Input: A file name
// Description: Reads in the input file, skips over Type and Count,
//              and reads in the maki count
// Output: does not return anything
void Game::makeDeck(string filename) {
        ifstream infile(filename);
        if(infile.fail()) {
        cerr << "Error opening file." << endl;
        exit(EXIT_FAILURE);
        }

        string sushiType;

        // Reads in file
        while(!infile.eof()) {
            int makiCount = 0;
            infile >> sushiType;
            if(sushiType == "Maki") {
                infile >> makiCount;
            }
            else if(sushiType == "Type") {
                infile >> sushiType;
                continue;
            }

            Card* p = new Card(sushiType, makiCount);
            deck.push_back(p);
        }

        // last card got duplicated, so delete it
        delete deck.back();
        deck.pop_back();

    }

//main game loop
void Game::playGame(){
    int card_index;
    string play_on;
    int startPoint = deck.size() - 1;

    //play three rounds
    for(int i = 0; i < PLAYER_COUNT; i++) {
        playerTotalScore[i] = 0;
        playerTotalPudding[i] = 0;
    }

    for(int round = 0; round < ROUNDS; round++){
        /*   TODO (Part I): Deal 9 cards to each player          */
        bool validPlay = dealCards(&startPoint);
        if(validPlay == false) {
            cerr << "ERROR: Not enough cards to play!" << endl;
            exit(EXIT_FAILURE);
        }


        int maxCardIndex = CARD_HAND;
        //select and pass all 27 cards
        for(int card = 0; card < CARD_HAND; card++){
            for(int player = 0; player < PLAYER_COUNT; player++){
                board.drawBoard(players, player);
                cout << " Player " << player + 1 << ", select a card: ";
                cin >> card_index;

                // Check if player enters a valid number
                while(card_index < 1 || card_index > maxCardIndex) {
                    cout << "     Please enter a valid number between 1 and ";
                    cout << maxCardIndex << ": ";
                    cin >> card_index;
                }

                /*   TODO (Part I): Set aside selected card      */  
                players[player].selectCard(card_index - 1);

            }
            /*   TODO (Part I): Reveal selected cards            */
            /*   TODO (Part I): Pass deck to the right           */
            for(int player = 0; player < PLAYER_COUNT; player++) {
                Vector* pHands = players[player].getPassingHand();
                int nextPlayer = (player + 1) % PLAYER_COUNT;
                players[nextPlayer].setPassingHand(pHands);
                maxCardIndex = pHands->size();
            }
            for(int player = 0; player < PLAYER_COUNT; player++) {
                players[player].setForDisplay();
            }
        }
        
        checkMaki();

        for(int player = 0; player < PLAYER_COUNT; player++) {
            playerTotalScore[player] += players[player].getScore();
            playerTotalPudding[player] += players[player].getPuddingCount();
            players[player].setPlayerTotScore(playerTotalScore[player]);
            players[player].setTotPuddingCount(playerTotalPudding[player]);
            players[player].setEndOfRound(false);
        }

        /*   TODO (Part II): Remove the above break statement    */
        /*   TODO (Part II): Update Scores                       */
        /*   TODO (Part II): Call board.drawScore(players);      */
        board.drawScore(players);
        /*   TODO (Part II): Prompt to go on to next round       */
        if(round < 2){
            cout << " End of round! Ready for Round "
                 << round + 2 << " ? (y/n): ";
            cin >> play_on;
            if(play_on != "y"){
                break;
            }
        }    
        /*   TODO (Part II): Reset for next round                */
        if(round < ROUNDS - 1) {
            for(int player = 0; player < PLAYER_COUNT; player++) {
                players[player].reset();
            }
        }  
    }

    /*   TODO (Part II): Score pudding and determine winner      */
    checkPudding();
    int maxIndex = 0;

    for(int i = 0; i < PLAYER_COUNT; i++) {
        if(playerTotalScore[maxIndex] < playerTotalScore[i]) {
            maxIndex = i;
        }
        else if(playerTotalScore[maxIndex] == playerTotalScore[i]) {
            if(playerTotalPudding[maxIndex] < playerTotalPudding[i]) {
                maxIndex = i;
            }
        }
    }

    if(playerTotalScore[0] == playerTotalScore[1]) {
        if(playerTotalScore[1] == playerTotalScore[2]) {
            maxIndex = -1;
        }
    }

    /*   TODO (Part II): Call board.drawWinner(players, winner); */
    for(int player = 0; player < PLAYER_COUNT; player++) {
        players[player].setEndOfRound(false);
    }
    int winner = maxIndex;
    board.drawWinner(players, winner);
}

// dealCards
// Input: int pointer to the start point
// Description: deals the cards to each player starting from the back of 
//              the deck
// Output: returns a bool corresponding to if there are enough cards to deal
bool Game::dealCards(int* startPoint) {
    int cardsPerRound = PLAYER_COUNT * CARD_HAND;

    if(*startPoint - cardsPerRound < 0) {
        return false;
    }

    // If the card is chopsticks, the card gets ignored, but player decrements
    // to make sure that the player does not get ignored.
    for(int card = 0; card < CARD_HAND; card++) { 
        for(int player = 0; player < PLAYER_COUNT; player++) {
            Card* p = deck.at(*startPoint);
            if(p->getSushiType() != "Chopsticks") {
                players[player].dealCard(p);
            }
            else {
                player--;
            }
            (*startPoint)--;
        }
    }

    return true;
}

// sortCounts
// Input: int pointer to the maki or pudding counts array before 
//        they are sorted
// Description: sort the counts from greatest to least
// Output: returns pointer to a int array of the sorted counts
int* Game::sortCounts(int* ogCounts) {
    static int sortedCount[PLAYER_COUNT];

    for(int i = 0; i < PLAYER_COUNT; i++) {
        sortedCount[i] = 0;
    }

    // sets contents in sortedCounts to contents in ogCounts
    for(int player = 0; player < PLAYER_COUNT; player++) {
        sortedCount[player] = ogCounts[player];
    }

    // sorts the sortedCounts from greates to least
    for(int j = 0; j < PLAYER_COUNT; j++) {
        int maxIndex = j;
        
        for(int k = j + 1; k < PLAYER_COUNT; k++) {
            if(sortedCount[k] > sortedCount[maxIndex]) {
                maxIndex = k;
            }
        }
    
        int temp = sortedCount[j];
        sortedCount[j] = sortedCount[maxIndex];
        sortedCount[maxIndex] = temp;
    } 

    return sortedCount;
}

// sortedPlayerNums
// Input: int pointer to the maki or pudding counts array before 
//        they are sorted and after they are sorted
// Description: reorders the player numbers to how the maki or pudding counts
//              have been sorted
// Output: returns pointer to a int array of the sorted player numbers
int* Game::sortedPlayerNums(int* sortedCount, int* ogCount) {
    static int playerNums[PLAYER_COUNT];

    for(int player = 0; player < PLAYER_COUNT; player++) {
        playerNums[player] = -1;
    }

    // reorders the player numbers according to the sorted counts
    for(int i = 0; i < PLAYER_COUNT; i++) {
        for(int j = 0; j < PLAYER_COUNT; j++) {
            if(sortedCount[i] == ogCount[j]) {
                bool check = false;
                // if 2 of the sorted counts are the same and skips the
                // player number corresponding to when it first appeared
                for(int k = 0; k < i; k++) {
                    if(playerNums[k] == j) {
                        check = true;
                        break;
                    }
                }

                if(check == false) {
                    playerNums[i] = j;
                    break;
                }
                else continue;
            }
        }
    }

    return playerNums;
}

// makiScoring
// Input: int pointer to the maki counts array after they are
//        sorted and an int pointer to the reordered player numbers
// Description: calculates the maki score for each player and stores it
//              in the score array
// Output: returns pointer to a int array of the players' maki scores
int* Game::makiScoring(int* makiCounts, int* playerNums) {
    static int score[PLAYER_COUNT];

    for(int i = 0; i < PLAYER_COUNT; i++) {
        score[i] = 0;
    }

    // calculates the maki score depending on each player's maki counts
    if(makiCounts[0] == makiCounts[1] && makiCounts[1] == makiCounts[2]) {
        score[playerNums[0]] = 6 / 3;
        score[playerNums[1]] = 6 / 3;
        score[playerNums[2]] = 6 / 3;
    }
    else {
        if(makiCounts[0] == makiCounts[1]) {
            score[playerNums[0]] = 6 / 2;
            score[playerNums[1]] = 6 / 2;
        }
        else {
            score[playerNums[0]] = 6;
            if(makiCounts[1] == makiCounts[2]) {
                score[playerNums[1]] = 3 / 2;
                score[playerNums[2]] = 3 / 2;
            }
            else {
                score[playerNums[1]] = 6 / 2;
            }
        }
    }

    return score;
}

// puddingScoring
// Input: int pointer to the pudding counts array after they are
//        sorted and an int pointer to the reordered player numbers
// Description: calculates the pudding score for each player and stores
//              it in the score array
// Output: returns pointer to a int array of the players' pudding scores
int* Game::puddingScoring(int* puddingCounts, int* playerNums) {
    static int score[PLAYER_COUNT];
    for(int i = 0; i < PLAYER_COUNT; i++) {
        score[i] = 0;
    }

    // calculates the pudding score depending on each player's maki counts
    if(puddingCounts[0] == puddingCounts[1] &&
       puddingCounts[1] == puddingCounts[2]) {
            score[playerNums[0]] = 0;
            score[playerNums[1]] = 0;
            score[playerNums[2]] = 0;
    }
    else {
        if(puddingCounts[0] == puddingCounts[1]) {
            score[playerNums[0]] = 6 / 2;
            score[playerNums[1]] = 6 / 2;
            score[playerNums[2]] = -6;
        }
        else {
            score[playerNums[0]] = 6;
            if(puddingCounts[1] == puddingCounts[2]) {
                score[playerNums[1]] = (-6) / 2;
                score[playerNums[2]] = (-6) / 2;
            }
            else score[playerNums[2]] = -6;
        }
    }
    return score;
}

// checkMaki
// Input: no inputs
// Description: sets each player's maki scores and calls setEndOfRound
//              so that checkEndOfRound is set to true.
// Output: does not return anything
void Game::checkMaki() {
    int ogMakiCounts[PLAYER_COUNT];

    for(int i = 0; i < PLAYER_COUNT; i++) {
        ogMakiCounts[i] = 0;
    }

    // gets each player's maki counts and stores it in ogMakiCounts
    for(int player = 0; player < PLAYER_COUNT; player++) {
        ogMakiCounts[player] = players[player].getMakiCount();
    }

    int* makiCounts = sortCounts(ogMakiCounts);
    int* playerNums = sortedPlayerNums(makiCounts, ogMakiCounts);
    int* score = makiScoring(makiCounts, playerNums);

    // sets the maki score for each player and sets checkEndOfRound to true
    for(int player = 0; player < PLAYER_COUNT; player++) {
        players[player].setMakiScore(score[player]);
        players[player].setEndOfRound();
    }
}

// checkPudding
// Input: no inputs
// Description: sets the each player's pudding scores, calls setEndOfRound
//              so that checkEndOfRound is set to true, and adds the 
//              pudding core to the final score
// Output: does not return anything
void Game::checkPudding() {
    int ogPuddingCounts[PLAYER_COUNT];

    for(int i = 0; i < PLAYER_COUNT; i++) {
        ogPuddingCounts[i] = 0;
    }

    // gets each player's pudding counts and stores it in ogPuddingCounts
    for(int player = 0; player < PLAYER_COUNT; player++) {
        ogPuddingCounts[player] = players[player].getPuddingCount();
    }

    int* puddingCounts = sortCounts(ogPuddingCounts);
    int* playerNums = sortedPlayerNums(puddingCounts, ogPuddingCounts);
    int* score = puddingScoring(puddingCounts, playerNums);
    
    // sets the pudding score for each player, sets checkEndOfRound to true,
    // and adds the pudding score to the total score
    for(int player = 0; player < PLAYER_COUNT; player++) {
        players[player].setPuddingScore(score[player]);
        players[player].setEndOfRound();
        playerTotalScore[player] += score[player];
        players[player].setPlayerTotScore(playerTotalScore[player]);
    }
}


//destructor
Game::~Game(){
    /*-----------------------------------------------*/
    /*   TODO (Parts I and II): CLEAN UP THE GAME    */
    /*                                               */
    /*   Make sure you are passing valgrind.         */
    /*-----------------------------------------------*/ 
    for(int i = 0; i < deck.size(); i++) {
        delete deck.at(i);
    }
}
