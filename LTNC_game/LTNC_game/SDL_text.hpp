//
//  SDL_text.hpp
//  LTNC_game
//
//  Created by Phan Việt Hưng on 27/04/2022.
//

#ifndef SDL_text_hpp
#define SDL_text_hpp
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

class LTexture
{
    public:
        //Initializes variables
        LTexture();

        //Deallocates memory
        ~LTexture();

        //Loads image at specified path
        bool loadFromFile( std::string path , SDL_Renderer *gRenderer);
        
        //Creates image from font string
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor, SDL_Renderer * gRenderer);

        //Deallocates texture
        void free();

        //Set color modulation
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        //Set blending
        void setBlendMode( SDL_BlendMode blending );

        //Set alpha modulation
        void setAlpha( Uint8 alpha );
        
        //Renders texture at given point
        void render( int x, int y, SDL_Renderer * gRenderer, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

        //Gets image dimensions
        int getWidth();
        int getHeight();
    
        //The actual hardware texture
        SDL_Texture* mTexture;

        //Image dimensions
        int mWidth;
        int mHeight;
};

//Loads media
bool loadMedia(LTexture &gTextTexture, SDL_Renderer * gRenderer, std::string text);

//Frees media and shuts down SDL
void close(LTexture gTextTexture, SDL_Renderer * gRenderer, SDL_Window * gWindow);
void getPlayerMoneyTexture(int money, SDL_Renderer *renderer, LTexture &player_money);

#endif /* SDL_text_hpp */
