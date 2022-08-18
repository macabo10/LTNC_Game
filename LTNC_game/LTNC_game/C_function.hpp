//
//  C_function.hpp
//  LTNC_game
//
//  Created by Phan Việt Hưng on 01/04/2022.
//

#ifndef C_function_hpp
#define C_function_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <sstream>
#include <iomanip>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "SDL_function.hpp"
using namespace std;

//Chất bài (bích cơ rô tép)
enum card_suit{
    S, H, D, C
};

//struct acount lưu trữ dự liệu về người chơi
struct account{ //account structure. This will store all the player information
    string name; //tên người chơi
    string email; //email
    string username; //username của người chơi
    string password; //password của người chơi

    //tiền của người chơi
    //trường hợp người chơi chơi trở lại tiền vẫn sẽ được lưu trữ
    int money;

    //các thông số người chơi
    int wins; //số ván thắng
    int total_played; //số ván đã chơi
    double win_percentage; //tỉ lệ thắng
};

//thông tin lá bài
struct card{
    int value; //giá trị số của lá bài
    card_suit suit; //chất của lá bài
    bool up; //kiểm tra lá bài úp hay ngửa (sử dụng cho dealer vì 1 lá của dealer sẽ phải để úp)
//    card(int _value, card_suit _suit, bool _up) {
//        value = _value;
//        suit = _suit;
//        up = _up;
//    }
};

//struct thông tin về người chơi trong game
//có bao gồm cả dealer
struct player{
    account info; //thông tin account của người chơi
    vector<card> hand; //những lá trên tay người chơi
    int bet; //mức bet trong ván của người chơi
    int insurance_bet; //mức bet nếu người chơi sử dụng bảo hiểm
    vector<SDL_Texture*> loadPlayerHandTexture(vector<vector<SDL_Texture* > > fullDeck, SDL_Renderer* renderer);
    vector<SDL_Texture*> handTexture;
};

struct texture_info {
    SDL_Texture* tex;
    int x;
    int y;
    int w;
    int h;
    texture_info(SDL_Texture* _tex ,int _x, int _y) {
        tex = _tex;
        x = _x;
        y = _y;
    }
};


card deal(vector<card> &usedCards);
card hitMe(vector<card> &usedCards);
bool checkDuplicatedCard (vector<card> usedCards, card newCard);
void clear(vector<card>&);

account load();
void save(player);
account create();
void renderPlayersCards(vector<player>& players, SDL_Renderer *renderer, vector<vector<SDL_Texture* > > fullDeck, int SCREEN_WIDTH);
void renderCurrentTexture(vector<texture_info> current_textures_info, SDL_Renderer *renderer);
#endif /* C_function_hpp */
