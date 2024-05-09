#include <time.h>
#include <stdio.h>
#include "ui.h"
#include "spider.h"

clock_t start, end, elapsed;

int SDL_main(int argc, char* argv[]) {
    start = clock();
    InitUI();
    InitCards(1);
    Update(0);
    while ( game_running ) {
        end = clock();
        elapsed = (end-start)/CLK_TCK;
        Update(elapsed);
        while (SDL_PollEvent(&MainEvent)) {
            switch (MainEvent.type) {
                case SDL_QUIT:
                    game_running = false;
                    Quit();
                    break;
                case SDL_KEYDOWN:
                    switch (MainEvent.key.keysym.sym) {
                        case SDLK_1:
                            start = clock();
                            InitCards(1);
                            break;
                        case SDLK_2:
                            start = clock();
                            InitCards(2);
                            break;
                        case SDLK_4:
                            start = clock();
                            InitCards(4);
                            break;
                        case SDLK_RETURN:
                            DealCards();
                            Step();
                            break;
                        case SDLK_BACKSPACE:
                            Regret();
                            Step();
                        case SDLK_SPACE:
                            GetHint();
                            Step();
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mouse_down_x = MainEvent.button.x;
                    mouse_down_y = MainEvent.button.y;
                    if ( mouse_down_y > 20 && mouse_down_y < 160 && mouse_down_x > 75+30*(deals-1) && mouse_down_x < 75+30*(deals-1)+100 ) {
                        DealCards();
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    mouse_up_x = MainEvent.button.x;
                    mouse_up_y = MainEvent.button.y;
                    MoveCards(GetColumn(mouse_down_x), GetColumn(mouse_up_x), GetCardIdx(mouse_down_x, mouse_down_y));
                    Step();
                    break;
                default:
                    break;
            }
        }
    }
}

