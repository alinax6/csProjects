/*
 * board.cpp
 * Alina Xie
 * Date: 12/1/2023
 */

#include "board.h"
#include "player.h"
#include "card.h"
#include "vector.h"

//constructor
Board::Board(){
    clearBoard();
}

/* drawBoard
 * Input: A pointer to an array of 3 Players, p
 *        A number indicating the index of the player whose turn
 *        it currently is (0, 1, or 2), turn
 * Description: Draws all of board elements for a given players turn
 * Output: Prints the board to standard output
 */
void Board::drawBoard(Player *players, int turn){
    //reset the screen
    clearBoard();
    screen_clear();
    screen_home();
    
    for(int i = 0; i < PLAYER_COUNT; i++){
        if(turn >= 0){
            drawPlayer(i, players + i, turn);
        }else{
            drawRound(i, players + i);
        }
    }

    //print out the finished board
    for(int i = 0; i < BOARD_HEIGHT - 6; i++){
        for(int j = 0; j < BOARD_WIDTH; j++){
            if(board_color[i][j] != "white"){
                screen_fg(board_color[i][j]);
            }
            cout << board[i][j];
            screen_fg("white");
        }
        cout << endl;
   } 
   cout << endl; 
}

/* drawScore
 * Input: A pointer to an array of 3 Players, p
 * Description: Draws a summary of the round's scoring
 * Output: Prints the summary to standard output
 */
void Board::drawScore(Player *players){
    drawBoard(players, -1);
}

//draws a player's chosen and unplayed hands
void Board::drawPlayer(int p_index, Player *player, int turn){
    int chosen_size;
    bool myTurn = (p_index == turn) ? true : false;
    int vpos = (p_index * (CARD_HEIGHT + 3)) + p_index;
    Vector *p_hand = player->getPassingHand();
    Vector *r_hand = player->getRevealedCards();

    //draw player heading
    vpos++;
    string label = "Player " + to_string(p_index + 1);
    drawLabel(vpos, 4, label);
    label = "Total Points: " + to_string(player->getScore());
    drawLabel(vpos, 20, label);
    label = "Total Puddings: " + to_string(player->getPuddingCount());
    drawLabel(vpos, 40, label);
    vpos++;

    //draw the chosen cards that have been revealed to all players
    chosen_size = r_hand->size();
    for(int i = 0; i < chosen_size; i++){
        drawCard(vpos, (i * CARD_WIDTH) + 1, i + 1, r_hand->at(i), false);
    }
    
    //draw the unselected cards in the passing hand
    for(int i = 0; i < p_hand->size(); i++){
        int hpos = ((i + chosen_size) * CARD_WIDTH) + 1;
        if(myTurn){
            drawCard(vpos, hpos, i + 1, p_hand->at(i), true);
        }else{
            drawCard(vpos, hpos, 0, nullptr, true);
        }
    }
}

//draws a summary of a players played cards
void Board::drawRound(int p_index, Player *player){
    int vpos = (p_index * (CARD_HEIGHT + 3)) + p_index;
    int hpos = 1;
    int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    string label;
    Vector *p_deck = player->getRevealedCards();

    //draw player heading
    vpos++;
    label = "Player " + to_string(p_index + 1);
    drawLabel(vpos, 4, label);
    label = "Total Points: " + to_string(player->getScore());
    drawLabel(vpos, 20, label);
    label = "Total Puddings: " + to_string(player->getPuddingCount());
    drawLabel(vpos, 40, label);
    vpos--;

    //draw pudding total
    int pudding_count = 0;
    for(int i = 0; i < p_deck->size(); i++){
        if(p_deck->at(i)->getSushiType() == "Pudding"){
            pudding_count++;
        }
    }

    if(pudding_count > 0){
        drawPudding(vpos, hpos);
        label = "x" + to_string(pudding_count);
        drawLabel(vpos + CARD_HEIGHT, hpos + CARD_WIDTH/2 - 1, label);
        hpos += CARD_WIDTH;
    }

    //draw maki total
    int maki_count = 0;
    for(int i = 0; i < p_deck->size(); i++){
        if(p_deck->at(i)->getSushiType() == "Maki"){
            maki_count += p_deck->at(i)->getMakiCount();
        }
    }

    if(maki_count > 0){
        drawMaki(vpos, hpos, -1);
        label = "x" + to_string(maki_count);
        drawLabel(vpos + CARD_HEIGHT, hpos + CARD_WIDTH/2 - 1, label);
        hpos += CARD_WIDTH;
    }

    //draw tempura total
    int tempura_count = 0;
    for(int i = 0; i < p_deck->size(); i++){
        if(p_deck->at(i)->getSushiType() == "Tempura"){
            tempura_count++;
        }
    }

    if(tempura_count > 0){
        drawTempura(vpos, hpos);
        label = "x" + to_string(tempura_count);
        drawLabel(vpos + CARD_HEIGHT, hpos + CARD_WIDTH/2 - 1, label);
        hpos += CARD_WIDTH;
    }

    //draw sashimi total
    int sashimi_count = 0;
    for(int i = 0; i < p_deck->size(); i++){
        if(p_deck->at(i)->getSushiType() == "Sashimi"){
            sashimi_count++;
        }
    }

    if(sashimi_count > 0){
        drawSashimi(vpos, hpos);
        label = "x" + to_string(sashimi_count);
        drawLabel(vpos + CARD_HEIGHT, hpos + CARD_WIDTH/2 - 1, label);
        hpos += CARD_WIDTH;
    }

    //draw dumpling total
    int dumpling_count = 0;
    for(int i = 0; i < p_deck->size(); i++){
        if(p_deck->at(i)->getSushiType() == "Dumpling"){
            dumpling_count++;
        }
    }

    if(dumpling_count > 0){
        drawDumpling(vpos, hpos);
        label = "x" + to_string(dumpling_count);
        drawLabel(vpos + CARD_HEIGHT, hpos + CARD_WIDTH/2 - 1, label);
        hpos += CARD_WIDTH;
    }

    //draw nigiri total
    for(int i = 0; i < p_deck->size(); i++){
        bool wasabi = false;
        string type = p_deck->at(i)->getSushiType();

        if(type == "Egg-Nigiri" ||
           type == "Salmon-Nigiri" ||
           type == "Squid-Nigiri"){
            //see if it can be dipped in wasabi
            for(int j = i - 1; j >= 0; j--){
                if(p_deck->at(j)->getSushiType() == "Wasabi" &&
                   mask[j] == 0){
                    mask[j] = 1;
                    wasabi = true;
                    break;
                }
            }

            drawNigiri(vpos, hpos, type, wasabi);
            hpos += CARD_WIDTH;
        }
    }
}

//draws text at the supplied location
void Board::drawLabel(int vpos, int hpos, string label){
    for(int i = 0; i < (int)label.length(); i++){
        board[vpos][hpos + i] = label[i];
    }
}

//draws a single sushi card
void Board::drawCard(int vpos, int hpos, int index, Card *c, bool unplayed){
    string top = "  _________  ";
    string second = " /         \\ ";
    string blank = "|           |";
    string bottom = " \\_________/ ";

    //draw the basic card outline
    if(unplayed){
        for(int i = 0; i < CARD_WIDTH; i++){
            board[vpos][hpos + i] = top[i];
            board[vpos + 1][hpos + i] = second[i];

            for(int j = 2; j < CARD_HEIGHT; j++){
                board[vpos + j][hpos + i] = blank[i];
            }
            board[vpos + CARD_HEIGHT][hpos + i] = bottom[i];
        }

        if(c == nullptr){
            drawBlank(vpos, hpos);
            return;
        }

        board[vpos + 1][hpos + 4] = '-';
        board[vpos + 1][hpos + 6] = index + 48;
        board[vpos + 1][hpos + 8] = '-';
    }

    string sushi_type = c->getSushiType();
    if(sushi_type == "Pudding"){
        drawPudding(vpos, hpos);
    }else if(sushi_type == "Wasabi"){
        drawWasabi(vpos, hpos);
    }else if(sushi_type == "Dumpling"){
        drawDumpling(vpos, hpos);
    }else if(sushi_type == "Sashimi"){
        drawSashimi(vpos, hpos);
    }else if(sushi_type == "Tempura"){
        drawTempura(vpos, hpos);
    }else if(sushi_type == "Maki"){
        drawMaki(vpos, hpos, c->getMakiCount());
    }else if(sushi_type == "Chopsticks"){
        drawChopsticks(vpos, hpos);
    }else if(sushi_type == "Squid-Nigiri"){
        drawNigiri(vpos, hpos, sushi_type, false);
    }else if(sushi_type == "Salmon-Nigiri"){
        drawNigiri(vpos, hpos, sushi_type, false);
    }else if(sushi_type == "Egg-Nigiri"){
        drawNigiri(vpos, hpos, sushi_type, false);
    }else{
        cerr << "ERROR: Unknown card type ("
             << c->getSushiType() << ")" << endl;
    }
}

//draws a face-down card
void Board::drawBlank(int vpos, int hpos){
    string top = "SUSHI";
    string bottom = " GO! ";

    int width = top.length();
    for(int i = 0; i < width; i++){
        board[vpos + 4][hpos + 4 + i] = top[i];
    }

    width = bottom.length();
    for(int i = 0; i < width; i++){
        board[vpos + 5][hpos + 4 + i] = bottom[i];
    }
}

//draws a nigiri icon
void Board::drawNigiri(int vpos, int hpos, string type, bool wasabi){
    vpos += 3;
    hpos += 2;
    string nigiri_img[4];
    nigiri_img[0] = "  ________";
    nigiri_img[1] = " /_/_/_/_/";
    nigiri_img[2] = "/       / ";
    nigiri_img[3] = "\\______/  ";

    int width = nigiri_img[0].length();
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < width; j++){
            board[vpos + i][hpos + j] = nigiri_img[i][j];
            if(i < 2){
                if(type == "Squid-Nigiri"){
                    board_color[vpos + i][hpos + j] = "blue";
                }else if(type == "Salmon-Nigiri"){
                    board_color[vpos + i][hpos + j] = "pink";
                }else if(type == "Egg-Nigiri"){
                    board_color[vpos + i][hpos + j] = "yellow";
                }
            }else{
                if(wasabi){
                    board_color[vpos + i][hpos + j] = "green";
                }
            }
        }
    }
}

//draws a chopsticks icon
void Board::drawChopsticks(int vpos, int hpos){
    vpos += 3;
    hpos += 3;
    string chopsticks_img[5];
    chopsticks_img[0] = "\\     /";
    chopsticks_img[1] = " \\   / ";
    chopsticks_img[2] = "  \\ /  ";
    chopsticks_img[3] = "   X   ";
    chopsticks_img[4] = "  / \\  ";
    

    int width = chopsticks_img[0].length();
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < width; j++){
            board[vpos + i][hpos + j] = chopsticks_img[i][j];
            board_color[vpos + i][hpos + j] = "yellow";
        }
    }
}

//draws a maki icon
void Board::drawMaki(int vpos, int hpos, int count){
    vpos += 2;
    hpos += 2;
    string maki_img[6];
    maki_img[0] = "  _____  ";
    maki_img[1] = " / ___ \\ ";
    maki_img[2] = "| /   \\ |";
    maki_img[3] = "| |   | |";
    maki_img[4] = "| \\___/ |";
    maki_img[5] = " \\_____/ ";

    int width = maki_img[0].length();
    for(int i = 0; i < 6; i++){
        for(int j = 0; j < width; j++){
            board[vpos + i][hpos + j] = maki_img[i][j];
            if(i < 1 || i > 4){
                board_color[vpos + i][hpos + j] = "green";
            }else if((i < 2 || i > 3) && (j < 2 || j > 6)){
                board_color[vpos + i][hpos + j] = "green";
            }else if(j < 1 || j > 7){
                board_color[vpos + i][hpos + j] = "green";
            }
        }
    }

    if(count > 0){
        board[vpos + 3][hpos + 4] = count + 48;
    }else{
        board[vpos + 3][hpos + 4] = '*';
    }
    board_color[vpos + 3][hpos + 4] = "pink";
}

//draws a tempura icon
void Board::drawTempura(int vpos, int hpos){
    vpos += 2;
    hpos += 3;
    string tempura_img[6];
    tempura_img[0] = "  __  ";
    tempura_img[1] = " /^^\\ ";
    tempura_img[2] = " \\^^^\\";
    tempura_img[3] = "  |^^|";
    tempura_img[4] = "   |^/";
    tempura_img[5] = "   VV ";

    int width = tempura_img[0].length();
    for(int i = 0; i < 6; i++){
        for(int j = 0; j < width; j++){
            board[vpos + i][hpos + j] = tempura_img[i][j];
            if(i > 4){
                board_color[vpos + i][hpos + j] = "pink";
            }else{
                board_color[vpos + i][hpos + j] = "yellow";
            }
        }
    }
}

//draws a pudding icon
void Board::drawPudding(int vpos, int hpos){
    vpos += 3;
    hpos += 2;
    string pudding_img[4];
    pudding_img[0] = "  __*__  ";
    pudding_img[1] = " /vvvvv\\ ";
    pudding_img[2] = "/vvvvvvv\\";
    pudding_img[3] = "|_______|";

    int width = pudding_img[0].length();
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < width; j++){
            board[vpos + i][hpos + j] = pudding_img[i][j];
            if(i < 3){
                board_color[vpos + i][hpos + j] = "blue";
            }else{
                board_color[vpos + i][hpos + j] = "yellow";
            }
        }
    }

    board_color[vpos][hpos + 4] = "red";
}

//draws a sashimi icon
void Board::drawSashimi(int vpos, int hpos){
    vpos += 2;
    hpos += 1;
    string sashimi_img[5];
    sashimi_img[0] = " _____     ";
    sashimi_img[1] = "|\\    \\__  ";
    sashimi_img[2] = "| \\____\\ \\ ";
    sashimi_img[3] = " \\|____|__\\";
    sashimi_img[4] = "    \\|____|";

    int width = sashimi_img[0].length();
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < width; j++){
            board[vpos + i][hpos + j] = sashimi_img[i][j];
            if(i > 3 || j > 7){
                board_color[vpos + i][hpos + j] = "red";
            }else{
                board_color[vpos + i][hpos + j] = "pink";
            }
        }
    }
    board_color[vpos + 1][hpos + 7] = "red";

}

//draws a wasabi icon
void Board::drawWasabi(int vpos, int hpos){
    vpos += 3;
    hpos += 2;
    string wasabi_img[4];
    wasabi_img[0] = "   (%)   ";
    wasabi_img[1] = "  (%%%)  ";
    wasabi_img[2] = " (%%%%%) ";
    wasabi_img[3] = "(%%%%%%%)";

    int width = wasabi_img[0].length();
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < width; j++){
            board[vpos + i][hpos + j] = wasabi_img[i][j];
            board_color[vpos + i][hpos + j] = "green";
        }
    }
}

//draws a dumpling icon
void Board::drawDumpling(int vpos, int hpos){
    vpos += 3;
    hpos += 2;
    string wasabi_img[4];
    wasabi_img[0] = " /^^^^^\\ ";
    wasabi_img[1] = "| o   o |";
    wasabi_img[2] = "| \\___/ |";
    wasabi_img[3] = " \\_____/ ";

    int width = wasabi_img[0].length();
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < width; j++){
            board[vpos + i][hpos + j] = wasabi_img[i][j];
            if(i < 3 && j > 1 && j < width - 2){
                board_color[vpos + i][hpos + j] = "yellow";
            }
        }
    }

}

//clear the board display
void Board::clearBoard(){
    for(int i = 0; i < BOARD_HEIGHT; i++){
        for(int j = 0; j < BOARD_WIDTH; j++){
            board[i][j] = ' ';
            board_color[i][j] = "white";
        }
   }  
}

//formats the point total to align nicely
string Board::formatScore(int score){
    if(score == 1){
        return " " + to_string(score) + " point ";
    }else if(score < 10 && score > -1){
        return " " + to_string(score) + " points";
    }else{
        return to_string(score) + " points";
    }
}

/* drawWinner
 * Input: A pointer to an array of 3 Players, p
 *        A number indicating the index of the player who won, winner
 *        (A winner value of -1 indicates a tie)
 * Description: Draws a final display indicating which player won the game
 * Output: Prints the display to standard output
 */
void Board::drawWinner(Player *players, int winner){
    int offset;
    string score = " FINAL SCORE        ";
    string p1 = " Player 1: " + formatScore(players[0].getScore());
    string p2 = " Player 2: " + formatScore(players[1].getScore());
    string p3 = " Player 3: " + formatScore(players[2].getScore());
    clearBoard();

    string player[6];
    player[0] = "__________.__                             "; 
    player[1] = "\\______   \\  | _____  ___.__. ___________ "; 
    player[2] = " |     ___/  | \\__  \\<   |  |/ __ \\_  __ \\"; 
    player[3] = " |    |   |  |__/ __ \\\\___  \\  ___/|  | \\/"; 
    player[4] = " |____|   |____(____  / ____|\\___  >__|   "; 
    player[5] = "                    \\/\\/         \\/       "; 

    string wins[6];
    wins[0] = "  __      __.__              ._.";
    wins[1] = " /  \\    /  \\__| ____   _____| |";
    wins[2] = " \\   \\/\\/   /  |/    \\ /  ___/ |";
    wins[3] = "  \\        /|  |   |  \\\\___ \\ \\|";
    wins[4] = "   \\__/\\  / |__|___|  /____  >__";
    wins[5] = "        \\/          \\/     \\/ \\/";

    string one[6];
    one[0] = "  ____  ";
    one[1] = " /_   | ";
    one[2] = "  |   | ";
    one[3] = "  |   | ";
    one[4] = "  |___| ";
    one[5] = "        ";

    string two[6];
    two[0] = " ________  ";
    two[1] = " \\_____  \\ ";
    two[2] = "  /  ____/ ";
    two[3] = " /       \\ ";
    two[4] = " \\_______ \\";
    two[5] = "         \\/";

    string three[6];
    three[0] = " ________  ";
    three[1] = " \\_____  \\ ";
    three[2] = "   _(__  < ";
    three[3] = "  /       \\";
    three[4] = " /______  /";
    three[5] = "        \\/ ";

    string lame[6];
    lame[0] = ".____                           ";
    lame[1] = "|    |   _____    _____   ____  ";
    lame[2] = "|    |   \\__  \\  /     \\_/ __ \\ ";
    lame[3] = "|    |___ / __ \\|  Y Y  \\  ___/ ";
    lame[4] = "|_______ (____  /__|_|  /\\___  >";
    lame[5] = "        \\/    \\/      \\/     \\/ ";

    string *winner_p;
    if(winner == 0){
        winner_p = one;
    }else if(winner == 1){
        winner_p = two;
    }else if(winner == 2){
        winner_p = three;
    }

    //print the final scores
    for(int j = 0; j < (int)score.length(); j++){
        board[1][j] = score[j];
        board[2][j] = p1[j];
        board[3][j] = p2[j];
        board[4][j] = p3[j];
    }
    offset = 25;

    if(winner < 0){
        //print the wins text
        for(int i = 0; i < 6; i++){
            for(int j = 0; j < (int)lame[i].length(); j++){
                board[i][j + offset] = lame[i][j];
            }
        }
    }else{
        //print the player text
        for(int i = 0; i < 6; i++){
            for(int j = 0; j < (int)player[i].length(); j++){
                board[i][j + offset] = player[i][j];
            }
        }
        offset += player[0].length() + 1;

        //print the winner number
        for(int i = 0; i < 6; i++){
            for(int j = 0; j < (int)winner_p[i].length(); j++){
                board[i][j + offset] = winner_p[i][j];
            }
        }
        offset += winner_p[0].length();

        //print the wins text
        for(int i = 0; i < 6; i++){
            for(int j = 0; j < (int)wins[i].length(); j++){
                board[i][j + offset] = wins[i][j];
            }
        }
    }

    for(int i = 0; i < 6; i++){
        for(int j = 0; j < BOARD_WIDTH; j++){
            if(board_color[i][j] != "white"){
                screen_fg(board_color[i][j]);
            }
            cout << board[i][j];
            screen_fg("white");
        }
        cout << endl;
   } 
   cout << endl; 
}