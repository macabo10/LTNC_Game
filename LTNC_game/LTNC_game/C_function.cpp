//
//  C_function.cpp
//  LTNC_game
//
//  Created by Phan Việt Hưng on 01/04/2022.
//

#include "C_function.hpp"
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
#include "SDL_function.hpp"

vector<SDL_Texture*> player::loadPlayerHandTexture(vector<vector<SDL_Texture* > > fullDeck, SDL_Renderer* renderer) {
    vector<SDL_Texture*> handTexture;
    for (int i = 0; i < hand.size(); i++) {
        if (hand[i].up == true) {
            if (hand[i].suit == H) {
                handTexture.push_back(fullDeck[0][hand[i].value-1]);
            } else if (hand[i].suit == D) {
                handTexture.push_back(fullDeck[1][hand[i].value-1]);
            } else if (hand[i].suit == C) {
                handTexture.push_back(fullDeck[2][hand[i].value-1]);
            } else if (hand[i].suit == S) {
                handTexture.push_back(fullDeck[3][hand[i].value-1]);
            }
        } else {
            handTexture.push_back(loadTexture("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/card_deck/backcard.png", renderer));
        }
        
    }
    return handTexture;
}

//hàm bốc thêm bài
card hitMe(vector<card> &usedCards){
    return deal(usedCards);
}

//hàm chia 1 lá bài
card deal(vector<card> &usedCards){
    card new_card;
    
    do {
        new_card.value = 1 + rand() % 13; //random số
        //random chất
        int suit = rand() % 4;
        switch(suit){
            case 0:
                new_card.suit = S;
                break;
            case 1:
                new_card.suit = H;
                break;
            case 2:
                new_card.suit = D;
                break;
            case 3:
                new_card.suit = C;
                break;
        }

        new_card.up = true;

    } while (checkDuplicatedCard(usedCards, new_card));
    
    return new_card;
}

bool checkDuplicatedCard (vector<card> usedCards, card newCard){
    for (int i = 0; i < usedCards.size(); i++) {
        if (newCard.value == usedCards[i].value && newCard.suit == usedCards[i].suit) {
            return true;
        }
    }
    return false;
}

//tạo account cho người chơi mới
account create(){
    account user; //khởi tạo 1 account mới
    cout << "What is the username you'd like to use?" << endl;
    
    //khai báo thông tin file dữ liệu người chơi
    fstream input; //input
    string filename; //tên file

    do{
        cin >> user.username; //nhập usermame
        filename = user.username + ".dat"; //thêm đuôi cho tên file
        input.open(filename); //thử mở file có tên username người chơi đã nhập
        //kiểm tra xem username có bị trùng không
        if(!input.fail()){
            cout << "This username is already taken. Please choose another one." << endl;
        }
    }while(!input.fail()); //chạy vòng lặp đến khi người chơi chọn được tên username không bị trùng

    cout << "Please enter a password." << endl; //nhập password
    cin.ignore();//xoá bộ nhớ đệm
    getline(cin, user.password); //cho người chơi nhập password (sư dụng getline vì password được phép có dấy cách)
    cin.ignore(); //xoá bộ nhớ đệm

    cout << "Please enter your name." << endl; //hỏi tên người chơi
    getline(cin, user.name); //nhập tên người chơi (sử dụng getline vì tên có thể có dấu cách)

    cout << "Please enter your email address." << endl; //hỏi email address
    cin >> user.email; //cho người chơi nhập tên

    cout << "Please enter how much money you'd like to deposit" << endl; //hỏi lượng tiền người chơi muốn buy in
    cin >> user.money; //cho người chơi nhập số tiền muốn buy in

    //set tất cả các thông số người chơi về 0
    user.total_played = 0;
    user.wins = 0;
    user.win_percentage = 0;

    input.close(); //đóng file
    return user; //trả về thông tin 1 người chơi thuộc struct account
}

//lưu thông tin account
void save(player user){
    ofstream output; //output stream
    string filename = user.info.username + ".dat";

    output.open(filename, ios::out); //xoá file cũ

    if(!output){//if we can't open output
        cerr << "Output file will not open and data could not be saved. " << endl;
    }

    double percent = 0;
    if(user.info.total_played == 0){
        percent = 0;
    }
    else{
        percent = ((double)user.info.wins/user.info.total_played)*100;
    }
    output << user.info.username << endl << user.info.password << endl << user.info.name << endl << user.info.email << endl << user.info.money << endl << user.info.total_played << endl << user.info.wins << endl << setprecision (3) << percent; //lưu thông tin
    output.close();
}

//load thông tin người chơi cũ
account load(){
    string username, password;
    string filename;
    account user;
    ifstream input;

    //cho người chơi đăng nhập
    cout << "What is your username" << endl;
    cin >> username;
    filename = username + ".dat";
    input.open(filename);

    while (input.fail()) {
        cout << "Your username is incorrect, please re-enter it. " << endl;
        cin >> username;
        filename = username + ".dat";
        input.open(filename);
    }
    

    getline(input, user.username); //lấy username từ file data
    getline(input, user.password); //lấy password từ file data

    
    cout << "What is your password? " << endl;
    cin >> password;
    
    while (!(password == user.password)) {
        cout << "Wrong password! Please re-enter password. " << endl;
        cin >> password;
    }
    
 
    getline(input, user.name);
    getline(input, user.email);
    input >> user.money;
    input >> user.total_played;
    input >> user.wins;
    input >> user.win_percentage;
    
    input.close(); //đóng file
    return user; //trả về user
}

//bỏ hết bài trên tay
void clear(vector<card> &hand){
    hand.clear();
}

void renderPlayersCards(vector<player>& players, SDL_Renderer *renderer, vector<vector<SDL_Texture* > > fullDeck, int SCREEN_WIDTH) {
    players[0].handTexture = players[0].loadPlayerHandTexture(fullDeck, renderer);
    players[1].handTexture = players[1].loadPlayerHandTexture(fullDeck, renderer);
    renderPlayerCards(players[0].handTexture, (SCREEN_WIDTH/2) - 100*players[0].hand.size()/2 - 20*(players[0].hand.size()-1)/2, 265, renderer);
    renderPlayerCards(players[1].handTexture, (SCREEN_WIDTH/2) - 100*players[1].hand.size()/2 - 20*(players[1].hand.size()-1)/2, 750, renderer);
}

void renderCurrentTexture(vector<texture_info> current_textures_info, SDL_Renderer *renderer) {
    for (int i = 0; i < current_textures_info.size(); i++) {
        renderTexture(current_textures_info[i].tex, renderer, current_textures_info[i].x, current_textures_info[i].y);
    }
}
