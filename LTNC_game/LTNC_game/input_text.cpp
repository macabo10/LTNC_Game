//
//  input_text.cpp
//  LTNC_game
//
//  Created by Phan Việt Hưng on 05/05/2022.
//

#include "input_text.hpp"
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
#include <SDL_mixer.h>

string inputText(SDL_Renderer *gRenderer, LTexture &gInputTextTexture, vector<texture_info> current_textures_info, vector<player> &players,vector<vector<SDL_Texture* > > fullDeck, int x1, int x2,  int y) {
    //The current input text.
    std::string inputText = " ";
    Mix_Chunk *click = Mix_LoadWAV("/Users/phanviethung/Documents/Learn/UET/LTNC/Game/LTNC_game/LTNC_game/click.wav");
    if( !loadMedia(gInputTextTexture, gRenderer, inputText) )
    {
        cout << "Failed to load media!" << endl;
    }
    else
    {
        //Main loop flag
        bool quit = false;

        //Event handler
        SDL_Event e;

        //Set text color as black
        SDL_Color textColor = { 255, 255, 255, 0xFF };

        gInputTextTexture.loadFromRenderedText( inputText.c_str(), textColor, gRenderer );

        //Enable text input
        SDL_StartTextInput();

        //While application is running
        while( !quit )
        {
            //The rerender text flag
            bool renderText = false;

            //Handle events on queue
            while( SDL_PollEvent( &e ) != 0 )
            {
                //User requests quit
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }
                //Special key input
                else if( e.type == SDL_KEYDOWN )
                {
                    Mix_PlayChannel(-1, click, 0);
                    //Handle backspace
                    if( e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 )
                    {
                        //lop off character
                        inputText.pop_back();
                        renderText = true;
                    }
                    //Handle copy
                    else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
                    {
                        SDL_SetClipboardText( inputText.c_str() );
                    }
                    //Handle paste
                    else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
                    {
                        inputText = SDL_GetClipboardText();
                        renderText = true;
                    }
                    else if (e.key.keysym.sym == SDLK_RETURN) {
                        quit = true;
                        return inputText;
                    }
                }
                //Special text input event
                else if( e.type == SDL_TEXTINPUT )
                {
                    Mix_PlayChannel(-1, click, 0);
                    //Not copy or pasting
                    if( !( SDL_GetModState() & KMOD_CTRL && ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' || e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) ) )
                    {
                        //Append character
                        inputText += e.text.text;
                        renderText = true;
                    }
                }
            }

            //Rerender text if needed
            if( renderText )
            {
                //Text is not empty
                if( inputText != "" )
                {
                    //Render new text
                    gInputTextTexture.loadFromRenderedText( inputText.c_str(), textColor , gRenderer);
                }
                //Text is empty
                else
                {
                    //Render space texture
                    gInputTextTexture.loadFromRenderedText( " ", textColor , gRenderer);
                }
            }

            //Clear screen
            SDL_RenderClear( gRenderer );
            for (int i = 0; i < current_textures_info.size(); i++) {
                renderTexture(current_textures_info[i].tex, gRenderer, current_textures_info[i].x, current_textures_info[i].y);
            }
            renderPlayersCards(players, gRenderer, fullDeck, 1600);
            renderTexture(gInputTextTexture.mTexture, gRenderer, x1 + (int) ((x2-x1-gInputTextTexture.mWidth)/2), y);
            //Update screen
            SDL_RenderPresent( gRenderer );
        }
        
        //Disable text input
        SDL_StopTextInput();
    }
    return inputText;
}

