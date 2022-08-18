//
//  input_text.hpp
//  LTNC_game
//
//  Created by Phan Việt Hưng on 05/05/2022.
//

#ifndef input_text_hpp
#define input_text_hpp

#include <stdio.h>
#include "SDL_function.hpp"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include "C_function.hpp"
#include "SDL_text.hpp"
using namespace std;


string inputText(SDL_Renderer *gRenderer, LTexture &gInputTextTexture, vector<texture_info> current_textures_info, vector<player> &players,vector<vector<SDL_Texture* > > fullDeck, int x1, int x2, int y);
#endif /* input_text_hpp */
