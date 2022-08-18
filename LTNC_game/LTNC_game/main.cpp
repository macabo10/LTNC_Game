#include <iostream>
#include "SDL_function.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <sstream>
#include <iomanip>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "SDL_text.hpp"
#include "C_function.hpp"
#include "input_text.hpp"
using namespace std;

//khai báo
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
const string WINDOW_TITLE = "Blackjack";
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 1000;
SDL_Texture *background = nullptr, *menu = nullptr, *rule = nullptr, *chipset = nullptr, *insurance_background = nullptr, *bet_background = nullptr, *win_background = nullptr, *lose_background = nullptr, *tied_background = nullptr, *LHS = nullptr, *DHS = nullptr, *DLHS = nullptr, *HS = nullptr, *add_money = nullptr, *buy_in = nullptr, *split_background = nullptr;
void unload_SDL_and_Images();
vector<vector<SDL_Texture* > > fullDeck;
LTexture insurance_bet, player_bet, player_score, player_money;
SDL_Color textColor = { 255, 255, 255 };

//Mức bet tối thiểu (khai báo global để mỗi lần sử dụng ở trong 1 function không phải khởi tạo lại)
int MIN_BET = 5; //Giá trị bet tối thiểu là 5

vector<card> usedCards;
vector<texture_info> current_textures_info;
vector<player> players;

//prototypes
int score(vector<card>);
bool hasAce(vector<card>);
void doubleDown(player, player&);
void payout(player, player&);
void split(player&, player&);
void bet(player&);
int winner(player, player&);
void insurance(vector<player>&);
void play(vector<player>&);
void cheat_dealer_play(player&);
void dealer_play(player&);
void load_SDL_and_Imanges();
void unload_SDL_and_Imanges();
int scoreSplitHand(vector<card> , LTexture&);

int main(){
    initSDL(window, renderer, WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT);
    load_SDL_and_Imanges();
    Mix_Music *theme = Mix_LoadMUS("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/main_theme.wav");
    Mix_PlayMusic(theme, 100);
    SDL_RenderClear(renderer);
    renderTexture(menu, renderer, 0, 0, 1600, 1000);
    SDL_RenderPresent(renderer);
    
    let_playler_choose_to_play_or_rule(renderer, rule, menu);

    //tạo 1 bộ rand số ngẫu nhiên để đoạn sau sử dụng hàm rand()
    unsigned int time_seed = time(0);
    srand(time_seed);

    player user; //khởi tạo người chơi
    player dealer; //khởi tạo dealer
    
    dealer.info.username = "Dealer";
    players.push_back(dealer); //đẩy dealer vào vector chứa danh sách người chơi
    
    user.info.username = "Player";
    user.info.money = 0;
    user.info.total_played = 0;
    user.info.wins = 0;
    players.push_back(user);
    SDL_RenderClear(renderer);
    current_textures_info.clear();
    getPlayerMoneyTexture(players[1].info.money, renderer, player_money);
    current_textures_info.push_back(texture_info(buy_in, 0, 0));
    current_textures_info.push_back(texture_info(player_money.mTexture, 1410 + (int) ((135-player_money.mWidth)/2), 818));
    string buyInStr = inputText(renderer, player_bet, current_textures_info, players,fullDeck, 1435, 1515, 905);
    stringstream cinBuyIn(buyInStr);
    cinBuyIn >> players[1].info.money;
    
    bool continuePlay = false; //đánh dấu xem có phải ván đầu tiên người chơi chơi không
    int size = players.size(); //số người chơi + 1 (dealer)
    do{
        for(int i =1;i<players.size();i++){
            
            if(continuePlay && players.size() > 1){//if they have played at least once
                bool quit = false;
                SDL_Event e;
                while( !quit ) {
                    while( SDL_PollEvent( &e ) != 0 ) {
                        if ((e.type == SDL_QUIT) || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                            Mix_Chunk *click = Mix_LoadWAV("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/click.wav");
                            Mix_PlayChannel(-1, click, 0);
                            unload_SDL_and_Images();
                            exit(1);
                        }
                        if ((e.type == SDL_MOUSEBUTTONDOWN)) {
                            if (e.button.x >= 580 && e.button.x <= 762 && e.button.y >= 480 && e.button.y <= 584) {
                                Mix_Chunk *click = Mix_LoadWAV("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/click.wav");
                                Mix_PlayChannel(-1, click, 0);
                                save(players[i]); //lưu thông tin người chơi
                                players.erase(players.begin() + (i)); //xoá người chơi ra khỏi ván
                                i--;
                                unload_SDL_and_Images();
                                quit = true;
                                exit(1);
                            } else if (e.button.x >= 820 && e.button.x <= 1005 && e.button.y >= 480 && e.button.y <= 584) {
                                Mix_Chunk *click = Mix_LoadWAV("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/click.wav");
                                Mix_PlayChannel(-1, click, 0);
                                quit = true;
                            }
                        }
                    }
                }
            }
            
            if(players[i].info.money < 5){ //kiểm tra xem người chơi còn đủ tiền không
                int addMoney;
                while (addMoney != 0 && players[i].info.money < 5) {
                    SDL_RenderClear(renderer);
                    current_textures_info.clear();
                    getPlayerMoneyTexture(players[1].info.money, renderer, player_money);
                    current_textures_info.push_back(texture_info(add_money, 0, 0));
                    current_textures_info.push_back(texture_info(player_money.mTexture, 1410 + (int) ((135-player_money.mWidth)/2), 818));
                    string moneyAddStr = inputText(renderer, player_bet, current_textures_info, players,fullDeck, 1435, 1515, 905);
                    stringstream CIN(moneyAddStr);
                    CIN >> addMoney;
                    players[i].info.money += addMoney;
                }
                if (addMoney == 0) { //nếu người chơi không nạp thêm tiền
                    save(players[i]); //lưu thông tin người chơi
                    players.erase(players.begin() + (i)); //xoá người chơi ra khỏi ván
                    i--;
                    exit(1);
                }
            }

            
        }
        play(players); //chơi thôi
        continuePlay = true; //người chơi đã chơi xong 1 ván
    }while(players.size() > 1);
    unload_SDL_and_Images();
    exit(1);
    return 0;
}

void dealer_play(player &dealer){
    while ((score(dealer.hand) < 17) || (score(dealer.hand) == 17 && hasAce(dealer.hand))) {
        dealer.hand.push_back(hitMe(usedCards)); //dealer gets a card
    }
}

void cheat_dealer_play(player &dealer) {
    if (score(players[1].hand) >= 21) {
        dealer_play(dealer);
    } else {
        card cheatCard;
        if (score(dealer.hand) < 11 && score(dealer.hand) > 2) {
            card cheatCard2;
            cheatCard2.value = 16 - score(dealer.hand);
            cheatCard2.suit = S;
            cheatCard2.up = true;
            bool cont = true;
            if (checkDuplicatedCard(usedCards, cheatCard2) && cont) {
                cheatCard2.suit = H;
            } else if (cont) {
                dealer.hand.push_back(cheatCard2);
                cont = false;
            }
            if (checkDuplicatedCard(usedCards, cheatCard2) && cont) {
                cheatCard2.suit = D;
            } else if (cont) {
                dealer.hand.push_back(cheatCard2);
                cont = false;
            }
            if (checkDuplicatedCard(usedCards, cheatCard2) && cont) {
                cheatCard2.suit = C;
            } else if (cont) {
                dealer.hand.push_back(cheatCard2);
                cont = false;
            }
            if (checkDuplicatedCard(usedCards, cheatCard2) && cont) {
                cheatCard2.value -= 1;
                cheatCard2.suit = S;
            } else if (cont) {
                dealer.hand.push_back(cheatCard2);
                cont = false;
            }
            if (checkDuplicatedCard(usedCards, cheatCard2) && cont) {
                cheatCard2.suit = H;
            } else if (cont) {
                dealer.hand.push_back(cheatCard2);
                cont = false;
            }
            if (checkDuplicatedCard(usedCards, cheatCard2) && cont) {
                cheatCard2.suit = D;
            } else if (cont) {
                dealer.hand.push_back(cheatCard2);
                cont = false;
            }
            if (checkDuplicatedCard(usedCards, cheatCard2) && cont) {
                cheatCard2.suit = C;
            } else if (cont) {
                dealer.hand.push_back(cheatCard2);
                cont = false;
            }
        }
        
        cheatCard.value = score(players[1].hand) + (1 + rand() % (21 - score(players[1].hand))) - score(dealer.hand);
        cheatCard.suit = S;
        cheatCard.up = true;
        if (checkDuplicatedCard(usedCards, cheatCard)) {
            cheatCard.suit = H;
        } else {
            dealer.hand.push_back(cheatCard);
            return;
        }
        if (checkDuplicatedCard(usedCards, cheatCard)) {
            cheatCard.suit = D;
        } else {
            dealer.hand.push_back(cheatCard);
            return;
        }
        if (checkDuplicatedCard(usedCards, cheatCard)) {
            cheatCard.suit = C;
        } else {
            dealer.hand.push_back(cheatCard);
            return;
        }
        if (!checkDuplicatedCard(usedCards, cheatCard)) {
            dealer.hand.push_back(cheatCard);
            return;
        }
        dealer_play(dealer);
        return;
    }
}

void play(vector<player> &players){
    char choice; //dùng cho các loại input
    bet(players[1]);
    
    //chia lá đầu tiên cho dealer và úp lá đó
    players[0].hand.push_back(deal(usedCards));
    players[0].hand[0].up = false;

    //chia lần lượt mỗi người 1 lá cho đến khi đủ 2 lá mỗi người
    for(int i = 1;i< (players.size()*2);i++){
        players[(i%players.size())].hand.push_back(deal(usedCards));
    }

    
    //chơi game
    bool cont = true, checkInsurance = false;
    for(int i = 1;i<players.size();i++){
        do{
            if(players[0].hand[1].value == 1 && cont){ // nếu lá ngửa của dealer là 1 lá át
                insurance(players);
                if(score(players[0].hand) == 21){
                    checkInsurance = true;
                }
                cont = false;
            } else if (players[0].hand[1].value != 1 && cont) {
                insurance_bet.loadFromRenderedText("0", textColor, renderer);
                cont = true;
            }
            
            current_textures_info.clear();
            
            if(score(players[0].hand) <= 21){
                if(((players[i].hand[0].value >= 10 && players[i].hand[1].value >= 10) || players[i].hand[0].value == players[i].hand[1].value) && players[i].hand.size() == 2  && score(players[i].hand) == 10){ //2 lá giống nhau mà có tổng điểm bằng 10 thì được split hoặc double down
                    current_textures_info.push_back((texture_info(DLHS, 0, 0)));
                }
                else if(((players[i].hand[0].value >= 10 && players[i].hand[1].value >= 10) || (players[i].hand[0].value == players[i].hand[1].value)) && players[i].hand.size() == 2){ //2 lá giống nhau được split
                    current_textures_info.push_back((texture_info(LHS, 0, 0)));
                }
                else if(players[i].hand.size() == 2 && score(players[i].hand) >= 9 && score(players[i].hand) <= 11 && !(hasAce(players[i].hand))){ //tổng điểm từ 9 - 11 thì được double down
                    current_textures_info.push_back(texture_info(DHS, 0, 0));
                }
                else{//không có gì đặc biệt thì chỉ được hit hoặc stay
                    current_textures_info.push_back(texture_info(HS, 0, 0));
                }
                
                SDL_RenderClear(renderer);
                getPlayerMoneyTexture(players[1].info.money, renderer, player_money);
                score(players[1].hand);
                current_textures_info.push_back(texture_info(player_score.mTexture, 1370 + (int) ((155-player_score.mWidth)/2), 685));
                current_textures_info.push_back(texture_info(player_money.mTexture, 1365 + (int) ((165-player_money.mWidth)/2), 761));
                current_textures_info.push_back(texture_info(player_bet.mTexture, 1365 + (int) ((165-player_bet.mWidth)/2), 841));
                current_textures_info.push_back(texture_info(insurance_bet.mTexture, 1370 + (int) ((150-insurance_bet.mWidth)/2), 910));
                renderCurrentTexture(current_textures_info, renderer);
                
                renderPlayersCards(players, renderer, fullDeck, SCREEN_WIDTH);
                SDL_RenderPresent(renderer);
                
                SDL_Event e;
                bool quit = false;
                while (!quit) {
                    while( SDL_PollEvent( &e ) != 0 ) {
                        if( e.type == SDL_KEYDOWN ) {
                            Mix_Chunk *click = Mix_LoadWAV("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/click.wav");
                            Mix_PlayChannel(-1, click, 0);
                            if (e.key.keysym.sym == SDLK_l) {
                                choice = 'l';
                                quit = true;
                            } else if (e.key.keysym.sym == SDLK_d) {
                                choice = 'd';
                                quit = true;
                            } else if (e.key.keysym.sym == SDLK_h) {
                                choice = 'h';
                                quit = true;
                            } else {
                                choice = 's';
                                quit = true;
                            }
                        }
                    }
                }
                
                switch(choice){
                    //split
                    case 'l':
                    {
                        split(players[0], players[i]);
                        choice = 's';
                        break;
                    }
                    // double down
                    case 'd':
                    {
                        doubleDown(players[0], players[i]);
                        choice = 'S';
                        break;
                    }
                    // hit
                    case 'h':
                    {
                        players[i].hand.push_back(hitMe(usedCards));
                        break;
                    }
                        
                    default:
                        choice = 's';
                }
                if(score(players[i].hand) > 21){
                    choice = 'S';
                }
            }
        }while(!(choice == 'S' || choice == 's')); //tiếp tục vòng lặp cho đến khi người chơi chọn stay
    }
    
    if (score(players[1].hand) > 21) {
        players[0].hand[0].up = true;
        payout(players[0], players[1]);
        clear(players[0].hand);
        clear(players[1].hand);
        return;
    }
    
    if (players[1].info.total_played > 5 && ((double)players[1].info.wins/players[0].info.total_played)*100 > 40) {
        cout << "cheat mode" << endl;
        cheat_dealer_play(players[0]);
    }else {
        dealer_play(players[0]); //lượt chơi của dealer
    }
    
    players[0].hand[0].up = true; //lật bài dealere

    for(int i = 1;i<players.size();i++){
        if(score(players[i].hand) > 21){
            cout << "You busted! ";
        }
        int win = winner(players[0], players[i]);
        if(win == 1){
            players[i].info.wins += 1;
        }
        
        if (checkInsurance) {
            players[1].info.money += 2*players[1].insurance_bet;
        }
        
        payout(players[0],players[i]);
        clear(players[i].hand);
        players[i].info.total_played+=1;
    }

    clear(players[0].hand);
}

void insurance(vector<player> &players){
    int size = players.size();
    
    for(int i=1;i<size;i++){
        int bet;
        players[i].insurance_bet = 0;
        if((players[i].info.money-(players[i].bet/2)) >=0){ //kiểm tra xem còn tiền để có thể insurance bet không
            do{
                SDL_RenderClear(renderer);
                getPlayerMoneyTexture(players[1].info.money, renderer, player_money);
                score(players[1].hand);
                current_textures_info.clear();
                current_textures_info.push_back(texture_info(insurance_background, 0, 0));
                current_textures_info.push_back(texture_info(player_score.mTexture, 1370 + (int) ((155-player_score.mWidth)/2), 685));
                current_textures_info.push_back(texture_info(player_money.mTexture, 1365 + (int) ((165-player_money.mWidth)/2), 761));
                current_textures_info.push_back(texture_info(player_bet.mTexture, 1365 + (int) ((165-player_bet.mWidth)/2), 841));
                LTexture insurance_input;
                string insurance_bet_str = inputText( renderer, insurance_input, current_textures_info, players, fullDeck, 1375, 1515, 910);
                insurance_bet = insurance_input;
                stringstream insuranceBetInput(insurance_bet_str);
                insuranceBetInput >> bet;
                                       
            }while(!(bet <= (players[i].bet/2) && bet >=0));

            if(bet != 0){
                players[i].insurance_bet = bet;
                players[i].info.money-=bet;
            }
            
            SDL_RenderClear(renderer);
            insurance_bet.loadFromRenderedText(to_string(players[1].insurance_bet), textColor, renderer);
            current_textures_info.push_back(texture_info(insurance_bet.mTexture, 1370 + (int) ((150-insurance_bet.mWidth)/2), 910));
            renderCurrentTexture(current_textures_info, renderer);
            renderPlayersCards(players, renderer, fullDeck, SCREEN_WIDTH);
            SDL_RenderPresent(renderer);
            
        }
    }
}


void bet(player &user){
    SDL_RenderClear(renderer);
    current_textures_info.clear();
    getPlayerMoneyTexture(players[1].info.money, renderer, player_money);
    current_textures_info.push_back(texture_info(bet_background, 0, 0));
    current_textures_info.push_back(texture_info(player_money.mTexture, 1410 + (int) ((135-player_money.mWidth)/2), 818));
    string betStr = inputText(renderer, player_bet, current_textures_info, players,fullDeck, 1435,1515 , 905);
    stringstream CIN(betStr);
    CIN >> players[1].bet;
    players[1].info.money -= players[1].bet;
}

void split(player &dealer, player &user){
    player split; //tạo hand mới để split
    vector<player> playersSplit;
    playersSplit.push_back(dealer);
    
    split.bet = user.bet;
    user.info.money -= user.bet;
    split.hand.push_back(user.hand[0]);
    split.hand.push_back(deal(usedCards));
    user.hand[0] = deal(usedCards); //bốc cho người chơi lá mới
    split.info.username = user.info.username;
    playersSplit.push_back(split);
    playersSplit.push_back(user);

    char choice;
    LTexture split_hand_score;
    vector<player> currentPlayer;
        for (int i = 1; i < playersSplit.size(); i++) {
            do {
                SDL_RenderClear(renderer);
                current_textures_info.clear();
                getPlayerMoneyTexture(user.info.money, renderer, player_money);
                score(playersSplit[2].hand);
                scoreSplitHand(playersSplit[1].hand, split_hand_score);
                current_textures_info.push_back(texture_info(split_background, 0, 0));
                current_textures_info.push_back(texture_info(player_score.mTexture, 1370 + (int) ((155-player_score.mWidth)/2), 685));
                current_textures_info.push_back(texture_info(player_money.mTexture, 1365 + (int) ((165-player_money.mWidth)/2), 761));
                current_textures_info.push_back(texture_info(player_bet.mTexture, 1365 + (int) ((165-player_bet.mWidth)/2), 841));
                current_textures_info.push_back(texture_info(insurance_bet.mTexture, 1370 + (int) ((150-insurance_bet.mWidth)/2), 910));
                current_textures_info.push_back(texture_info(split_hand_score.mTexture, 1355 + (int) ((180-split_hand_score.mWidth)/2), 560));
                renderCurrentTexture(current_textures_info, renderer);
                currentPlayer.clear();
                currentPlayer.push_back(playersSplit[0]);
                currentPlayer.push_back(playersSplit[i]);
                renderPlayersCards(currentPlayer, renderer, fullDeck, SCREEN_WIDTH);
                SDL_RenderPresent(renderer);
                if(score(playersSplit[i].hand) > 21){
                    choice = 's';
                } else {
                    SDL_Event e;
                    bool quit = false;
                    while (!quit) {
                        while( SDL_PollEvent( &e ) != 0 ) {
                            if( e.type == SDL_KEYDOWN ) {
                                Mix_Chunk *click = Mix_LoadWAV("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/click.wav");
                                Mix_PlayChannel(-1, click, 0);
                                if (e.key.keysym.sym == SDLK_h) {
                                    choice = 'h';
                                    quit = true;
                                } else {
                                    choice = 's';
                                    quit = true;
                                }
                            }
                        }
                    }
                }
                if(choice == 'h'){
                    playersSplit[i].hand.push_back(hitMe(usedCards));
                }
            } while(!(choice == 'S' || choice == 's'));\
        }

    dealer_play(dealer);
    user = playersSplit[2];

    if (winner(dealer, playersSplit[1]) == 1) {
        user.info.money += (user.bet*2);
    } else if (winner(dealer, playersSplit[1]) == 0) {
        user.info.money += user.bet;
    }
}

void payout(player dealer, player &user){
    if(winner(dealer, user) == 1){ //người chơi thắng
        if(score(user.hand) == 21 && hasAce(user.hand) && user.hand.size() == 2){ //blackjack an 3/2
            user.info.money += ((user.bet*3)/2);
            user.bet = 0;
            SDL_RenderClear(renderer);
            current_textures_info.clear();
            current_textures_info.push_back(texture_info(win_background, 0, 0));
            getPlayerMoneyTexture(players[1].info.money, renderer, player_money);
            current_textures_info.push_back(texture_info(player_money.mTexture, 1425 + (int) ((120-player_money.mWidth)/2), 895));
            renderCurrentTexture(current_textures_info, renderer);
            renderPlayersCards(players, renderer, fullDeck, SCREEN_WIDTH);
            SDL_RenderPresent(renderer);
            user.info.wins += 1;
            
        }
        else{
            user.info.money+= (user.bet*2);
            user.bet = 0;
            SDL_RenderClear(renderer);
            current_textures_info.clear();
            current_textures_info.push_back(texture_info(win_background, 0, 0));
            getPlayerMoneyTexture(players[1].info.money, renderer, player_money);
            current_textures_info.push_back(texture_info(player_money.mTexture, 1425 + (int) ((120-player_money.mWidth)/2), 895));
            renderCurrentTexture(current_textures_info, renderer);
            renderPlayersCards(players, renderer, fullDeck, SCREEN_WIDTH);
            SDL_RenderPresent(renderer);
            user.info.wins += 1;
        }
    }
    else if (winner(dealer, user) == 0){ //hoà
        user.info.money+= user.bet;
        user.bet = 0;
        SDL_RenderClear(renderer);
        current_textures_info.clear();
        current_textures_info.push_back(texture_info(tied_background, 0, 0));
        getPlayerMoneyTexture(players[1].info.money, renderer, player_money);
        current_textures_info.push_back(texture_info(player_money.mTexture, 1425 + (int) ((120-player_money.mWidth)/2), 895));
        renderCurrentTexture(current_textures_info, renderer);
        renderPlayersCards(players, renderer, fullDeck, SCREEN_WIDTH);
        SDL_RenderPresent(renderer);
        
    }
    else{ //thua
        user.bet = 0;
        SDL_RenderClear(renderer);
        current_textures_info.clear();
        current_textures_info.push_back(texture_info(lose_background, 0, 0));
        getPlayerMoneyTexture(players[1].info.money, renderer, player_money);
        current_textures_info.push_back(texture_info(player_money.mTexture, 1425 + (int) ((120-player_money.mWidth)/2), 895));
        renderCurrentTexture(current_textures_info, renderer);
        renderPlayersCards(players, renderer, fullDeck, SCREEN_WIDTH);
        SDL_RenderPresent(renderer);
    }
}

int winner(player dealer, player &user){
    if (score(user.hand) > 21) {
        return -1;
    }
    if(score(dealer.hand) == score(user.hand)){
        return 0;
    }
    else if(((score(dealer.hand) < score(user.hand)) && (score(user.hand) <= 21)) || (score(dealer.hand) > 21 && score(user.hand) <= 21)){
        return 1;
    }
    else{
        return -1;
    }
}

void doubleDown(player dealer, player &user){
    user.info.money-=user.bet;
    user.bet*=2;
    user.hand.push_back(deal(usedCards)); //them bai
}

//check xem có át không
bool hasAce(vector<card> hand){
    for(int i =0;i<hand.size();i++){
        if(hand[i].value == 1){
            return true;
        }
    }
    return false;
}

// tính tổng điểm trên tay
int score(vector<card> hand){
    int total = 0; //tổng điểm
    for(int i = 0; i<hand.size();i++){
        if(hand[i].value >= 10){ //tính điểm với những lá đầu người hoặc lá 10
            total+=10;
        }
        else{
            total += hand[i].value; //tính điểm với những lá bình thường
        }
    }

    if(hasAce(hand) && total <= 11){ //nếu có át mà bài chưa cộng vào chưa quá 21 thì đổi át thành 11 điểm
        total+=10;
    }
    player_score.loadFromRenderedText(to_string(total), textColor, renderer);

    return total;
}

int scoreSplitHand(vector<card> hand, LTexture &split_hand_score) {
    int total = 0; //tổng điểm
    for(int i = 0; i<hand.size();i++){
        if(hand[i].value >= 10){ //tính điểm với những lá đầu người hoặc lá 10
            total+=10;
        }
        else{
            total += hand[i].value; //tính điểm với những lá bình thường
        }
    }

    if(hasAce(hand) && total <= 11){ //nếu có át mà bài chưa cộng vào chưa quá 21 thì đổi át thành 11 điểm
        total+=10;
    }
    split_hand_score.loadFromRenderedText(to_string(total), textColor, renderer);

    return total;
}

void load_SDL_and_Imanges() {
    menu = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/menu.png", renderer);
    rule = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/rule.PNG", renderer);
    background = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/play_background.png", renderer);
    fullDeck = loadFullDeck(renderer);
    chipset = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/chipset.png", renderer);
    insurance_background = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/insurance_background.png", renderer);
    bet_background = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/bet.png", renderer);
    win_background = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/win.png", renderer);
    lose_background = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/lose.png", renderer);
    tied_background = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/tied.png", renderer);
    LHS = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/LHS.png", renderer);
    DHS = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/DHS.png", renderer);
    DLHS = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/DLHS.png", renderer);
    HS = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/HS.png", renderer);
    add_money = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/add_Money.png", renderer);
    buy_in = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/buy_in.png", renderer);
    split_background = loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/split_background.png", renderer);
}

void unload_SDL_and_Images() {
    SDL_DestroyTexture(menu);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(rule);
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            SDL_DestroyTexture(fullDeck[i][j]);
        }
    }
    SDL_DestroyTexture(insurance_background);
    SDL_DestroyTexture(bet_background);
    SDL_DestroyTexture(win_background);
    SDL_DestroyTexture(lose_background);
    SDL_DestroyTexture(tied_background);
    SDL_DestroyTexture(LHS);
    SDL_DestroyTexture(DHS);
    SDL_DestroyTexture(DLHS);
    SDL_DestroyTexture(HS);
    SDL_DestroyTexture(add_money);
    SDL_DestroyTexture(buy_in);
    SDL_DestroyTexture(split_background);
    
}
