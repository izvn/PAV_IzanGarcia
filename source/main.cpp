#include <SDL.h>
#include <SDL_image.h>
#include <ctime>
#include <iostream>
#include "Game.h"
#include "InputManager.h"
#include "TimeManager.h"

int main(int argc, char* args[]) {
    Game game;
    srand((unsigned)time(NULL));
    try {
        game.Init();
    }
    catch (std::exception& exception) {
        std::cout << "Error: " << exception.what();
        game.Release();
        return -1;
    }

    bool playing = true;
    while (playing) {
        TIME.Update();
        playing = !Input.Listen();
        game.Update();
        game.Render();
        SDL_Delay(16); 
    }
    game.Release();
    return 0;
}
