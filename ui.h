#ifndef PROJECT_UI_H
#define PROJECT_UI_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "spider.h"

#define WIDTH 1600
#define HEIGHT 900

// Basic components
SDL_Event MainEvent;
SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL;

// Background
SDL_Surface *sur_main = NULL;
SDL_Texture *tex_main = NULL;
SDL_Rect rect_main;

// Cards
SDL_Surface *sur_cards[56];
SDL_Texture *tex_card = NULL;
SDL_Rect rect_card;

// Fonts
TTF_Font *font = NULL;
SDL_Surface *sur_font = NULL;
SDL_Texture *tex_font = NULL;
SDL_Rect rect_font;
SDL_Color color_font = {255, 255, 255, 255};

int mouse_down_x;
int mouse_down_y;
int mouse_up_x;
int mouse_up_y;
int total_cards[11];
char text_time[20];

void Update(int time);
int GetColumn(int pos_x);
int GetCardIdx(int pos_x, int pos_y);
void SetPos(int card, int pos_x, int pos_y);
void PrintTime(char* text);
void PrintMsg(char* text);
void PrintBackground();
void PrintCards();
void InitUI();
void Load();
void Quit();

int GetColumn(int pos_x) {
    return ((pos_x-75) % 150 > 100) ? -1 : (pos_x-75)/150;
}

int GetCardIdx(int pos_x, int pos_y) {
    int cards = total_cards[GetColumn(pos_x)+1];
    if ( pos_y < 180 || pos_y > 180 + 30 * cards + 90 ) { return 0; }
    if ( pos_y > 180 + 30 * (cards-1) && pos_y < 180 + 30 * (cards-1) + 140 ) { return 1; }
    return cards - (pos_y-180) / 30;
}

void Update(int time) {
    SDL_RenderClear(Renderer);
    sprintf(text_time, "Time Elapsed: %dm%ds.", time/60, time%60);
    PrintBackground();
    PrintCards();
    PrintMsg(msg);
    PrintTime(text_time);
    SDL_RenderPresent(Renderer);
}

void PrintBackground() {
    tex_main = SDL_CreateTextureFromSurface(Renderer, sur_main);
    SDL_RenderCopy(Renderer, tex_main, NULL, NULL);
    SDL_DestroyTexture(tex_main);
}

void PrintCards() {
    for ( int i=0; i<8; i++ ) {
        int color = (color_sets[i] != -1) ? 1+14*color_sets[i] : -1;
        SetPos(color, 375+150*i, 20);
    }
    for ( int i=0; i<deals; i++ ) { SetPos(0, 75+30*i, 20); }
    for ( int i=0; i<SLOTS; i++ ) {
        int cards = 0;
        for ( int j=0; j<slots[i].amount_hidden; j++ ) { SetPos(0, 75+150*i, 180+30*(cards++)); }
        for ( int j=0; j<slots[i].amount_revealed; j++ ) { SetPos(slots[i].revealed[j], 75+150*i, 180+30*(cards++)); }
        total_cards[i+1] = cards;
    }
}

void PrintTime(char* text) {
    sur_font = TTF_RenderUTF8_Blended(font, text, color_font);
    tex_font = SDL_CreateTextureFromSurface(Renderer, sur_font);
    rect_font.x = 1300;
    rect_font.y = 850;
    rect_font.w = sur_font->w;
    rect_font.h = sur_font->h;
    SDL_RenderCopy(Renderer, tex_font, NULL, &rect_font);
    SDL_DestroyTexture(tex_font);
}

void PrintMsg(char* text) {
    sur_font = TTF_RenderUTF8_Blended(font, text, color_font);
    tex_font = SDL_CreateTextureFromSurface(Renderer, sur_font);
    rect_font.x = 20;
    rect_font.y = 850;
    rect_font.w = sur_font->w;
    rect_font.h = sur_font->h;
    SDL_RenderCopy(Renderer, tex_font, NULL, &rect_font);
    SDL_DestroyTexture(tex_font);
}

void SetPos(int card, int pos_x, int pos_y) {
    tex_card = SDL_CreateTextureFromSurface(Renderer, sur_cards[card]);
    rect_card.x = pos_x;
    rect_card.y = pos_y;
    rect_card.w = 100;
    rect_card.h = 140;
    SDL_RenderCopy(Renderer, tex_card, NULL, &rect_card);
    SDL_DestroyTexture(tex_card);
}

void InitUI() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Window = SDL_CreateWindow("CosSky", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    Load();
}

void Load() {
    font = TTF_OpenFont("font/a.ttf", 36);
    sur_main = IMG_Load("img/bg.png");
    tex_main = SDL_CreateTextureFromSurface(Renderer, sur_main);
    rect_main.x = 0;
    rect_main.y = 0;
    rect_main.w = sur_main->w;
    rect_main.h = sur_main->h;
    for ( int i=-1; i<56; i++ ) {
        char card_name[20];
        if ( i < 1 || i % 14 ) {
            sprintf(card_name, "img/%d_.png", i);
            sur_cards[i] = IMG_Load(card_name);
        }
    }
}

void Quit() {
    for ( int i=-1; i<56; i++ ) {
        if ( i < 1 || i % 14 ) {
            SDL_FreeSurface(sur_cards[i]);
        }
    }
    SDL_FreeSurface(sur_main);
    SDL_FreeSurface(sur_font);
    SDL_DestroyTexture(tex_card);
    SDL_DestroyTexture(tex_main);
    SDL_DestroyTexture(tex_font);
    TTF_CloseFont(font);
    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_Quit();
}

#endif //PROJECT_UI_H
