//
//  SDL_function.hpp
//  LTNC_game
//
//  Created by Phan Việt Hưng on 26/03/2022.
//

#ifndef SDL_function_hpp
#define SDL_function_hpp

#include <stdio.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <sstream>
#include <SDL_image.h>
#include "SDL_text.hpp"
#include "C_function.hpp"

using namespace std;

vector<SDL_Texture *> loadCard(string suit, SDL_Renderer* renderer);
vector<vector<SDL_Texture *> > loadFullDeck(SDL_Renderer* renderer);
SDL_Texture* loadTexture(const string &file, SDL_Renderer *ren);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);
void waitUntilKeyPressed();

void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void logSDLError(std::ostream& os,
                 const std::string &msg, bool fatal);
void initSDL(SDL_Window* &window, SDL_Renderer* &renderer, const string title, const int screenWidth, const int screenHeight);

void renderPlayerCards(vector<SDL_Texture*> hand, int x, int y, SDL_Renderer* renderer);

int getPlayerChoicePlayOrRule();
void let_playler_choose_to_play_or_rule(SDL_Renderer *renderer, SDL_Texture *rule, SDL_Texture *menu);



#endif /* SDL_function_hpp */
