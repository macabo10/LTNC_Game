//
//  SDL_function.cpp
//  LTNC_game
//
//  Created by Phan Việt Hưng on 26/03/2022.
//

#include "SDL_function.hpp"
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <sstream>
using namespace std;


void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void logSDLError(std::ostream& os,
                 const std::string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if (fatal) {
        SDL_Quit();
        exit(1);
    }
}

vector<SDL_Texture *> loadCard(string suit, SDL_Renderer* renderer) {
    string folderPath = "/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/images/card_deck";
    vector<SDL_Texture *> cardVector;
    for (int i = 0; i < 13; i++) {
        int cardNum = i +1;
        string valueInStr = to_string(cardNum);
        cardVector.push_back(loadTexture(folderPath + "/" + valueInStr + suit + ".png", renderer));
    }
    return cardVector;
}

vector<vector<SDL_Texture *> > loadFullDeck(SDL_Renderer* renderer) {
    vector<vector<SDL_Texture *> > cardDeck;
    cardDeck.push_back(loadCard("H", renderer));
    cardDeck.push_back(loadCard("D", renderer));
    cardDeck.push_back(loadCard("C", renderer));
    cardDeck.push_back(loadCard("S", renderer));
    
    return cardDeck;
}


SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren) {
    SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
    if (texture == nullptr) {
        logSDLError(std::cout, "LoadTexture", true);
    }
    return texture;
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer, const string title, const int screenWidth, const int screenHeight) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
       SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
//    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
//       SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_FULLSCREEN_DESKTOP);
    //window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
    //   SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (window == nullptr) logSDLError(std::cout, "CreateWindow", true);


    //Khi thông thường chạy với môi trường bình thường ở nhà
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC);
    //Khi chạy ở máy thực hành WinXP ở trường (máy ảo)
    //renderer = SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(window));
    if (renderer == nullptr) logSDLError(std::cout, "CreateRenderer", true);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, screenWidth, screenHeight);
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
    //Thiết lập hình chữ nhật đích mà chúng ta muốn vẽ ảnh vào trong
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    //Truy vẫn texture để lấy chiều rộng và cao (vào chiều rộng và cao tương ứng của hình chữ nhật đích)
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    //Đưa toàn bộ ảnh trong texture vào hình chữ nhật đích
    SDL_RenderCopy(ren, tex, NULL, &dst);
}


void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h) {
    //Thiết lập hình chữ nhật đích mà chúng ta muốn vẽ ảnh vào trong
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    //Đưa toàn bộ ảnh trong texture vào hình chữ nhật đích
    //(ảnh sẽ co dãn cho khớp với kích cỡ mới)
    SDL_RenderCopy(ren, tex, NULL, &dst);
}

void waitUntilKeyPressed() {
    SDL_Event e;
    while (true) {
        if ( SDL_WaitEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}


