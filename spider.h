#ifndef PROJECT_SPIDER_H
#define PROJECT_SPIDER_H

#include "ui.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SLOTS 10
#define CARD_SET 13
#define TOTAL_CARDS 104

bool quit = 0;
bool failed = 0;
bool game_running = 0;

int output, hint;
int srch, desh, cnth;
int src_, des_, cnt_, open_;
int deals = 5, collected = 0, color_sets[8];
char last_req, bucket[14];
char* msg = "Started a new game - Easy!";

typedef struct {
    int heap[5], hidden[5], revealed[99];
    int max_successive[CARD_SET];
    int amount_heap, amount_hidden, amount_revealed;
    int movable, ms_low, ms_high;
} Slot; Slot slots[SLOTS];

int Max(int a, int b);
int Min(int a, int b);
void Swap(int* a, int* b);
void Shuffle(int *arr, int len);
int GetMaxCards();
void RevealHiddenCards();
void GetMaxSuccessive();
void EliminateCards();
void InitCards(int mode);
void JudgeWin();
void JudgeFail();
void Step();
void DealCards();
void MoveCards(int src, int des, int cnt);
void Regret();
void GetHint();

int Max(int a, int b) { return a>b ? a:b; }
int Min(int a, int b) { return a<b ? a:b; }
void Swap(int* a, int* b) { int t = *a; *a = *b; *b = t; }

void Shuffle(int *arr, int len) {
    srand(time(NULL));
    for ( int i=0; i<len; i++ ) {
        int tmp1 = (rand() % len), tmp2 = (rand() % len);
        if ( tmp1 != tmp2 ) { Swap(&arr[tmp1], &arr[tmp2]); }
    }
}

int GetMaxCards() {
    int ret = 0;
    for ( int i=0; i<SLOTS; i++ ) {
        ret = Max(ret, (slots[i].amount_hidden+slots[i].amount_revealed));
    }
    return ret;
}

void RevealHiddenCards() {
    open_ = ( last_req == 'M' ) ? 0:open_;
    for ( int i=0; i<SLOTS; i++ ) {
        if ( !slots[i].amount_revealed && slots[i].amount_hidden ) {
            slots[i].revealed[0] = slots[i].hidden[slots[i].amount_hidden-1];
            slots[i].amount_revealed++;
            slots[i].amount_hidden--;
            open_ = 1;
        }
    }
}

void GetMaxSuccessive() {
    for ( int i=0; i<SLOTS; i++ ) {
        for ( int j=0; j<CARD_SET; j++ ) { slots[i].max_successive[j] = 0; }
        int idx = 0;
        for ( int j=slots[i].amount_revealed; j>0; j-- ) {
            // Not first card.
            if ( idx ) {
                if ( slots[i].max_successive[idx-1] == slots[i].revealed[j-1] - 1  ) {
                    slots[i].max_successive[idx] = slots[i].revealed[j-1]; idx++;
                } else { break; }
            }
                // First card.
            else { slots[i].max_successive[idx] = slots[i].revealed[j-1]; idx++; }
        }
        // Update movable cards.
        slots[i].movable = idx;
        slots[i].ms_low = slots[i].max_successive[0];
        slots[i].ms_high = slots[i].max_successive[idx-1];
    }
}

void EliminateCards() {
    for ( int i=0; i<SLOTS; i++ ) {
        int first = slots[i].max_successive[0];
        int color = first / 14;
        int removable = 1;
        if ( first % 14 != 1 ) { removable = 0; }
        else {
            for ( int j=0; j<CARD_SET; j++ ) {
                if ( slots[i].max_successive[j] != j+1+14*color ) {
                    removable = 0;
                    break;
                }
            }
        }
        if ( removable ) {
            for ( int j=0; j<CARD_SET; j++ ) {
                slots[i].amount_revealed--;
                // TODO - Animations
            }
            color_sets[collected] = color;
            collected++;
        }
    }
}

void InitCards(int mode) {
    int pool_idx = 0;
    int card_pool[TOTAL_CARDS] = {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    };
    msg = "Started a new game - Easy!";
    if ( mode == 2 ) {
        msg = "Started a new game - Medium!";
        for ( int i=0; i<4; i++ ) {
            for ( int j=0; j<CARD_SET; j++ ) {
                card_pool[(4+i)*CARD_SET+j] = 15 + j;
            }
        }
    } else if ( mode == 4 ) {
        msg = "Started a new game - Hard!";
        for ( int k=0; k<3; k++ ) {
            for ( int i=0; i<2; i++ ) {
                for ( int j=0; j<CARD_SET; j++ ) {
                    card_pool[(2*(k+1)+i)*CARD_SET+j] = 15 + 14 * k + j;
                }
            }
        }
    }
    Shuffle(card_pool, TOTAL_CARDS);
    for ( int i=0; i<SLOTS; i++ ) {
        slots[i].amount_heap = 5;
        slots[i].amount_revealed = 1;
        slots[i].amount_hidden = ( i<4 ? 5:4 );
        slots[i].revealed[0] = card_pool[pool_idx++];
        for ( int j=0; j<5; j++ ) {
            slots[i].heap[j] = card_pool[pool_idx++];
        }
        for ( int j=0; j<slots[i].amount_hidden; j++ ) {
            slots[i].hidden[j] = card_pool[pool_idx++];
        }
    }
    for ( int i=0; i<8; i++ ) {
        color_sets[i] = -1;
    }
    deals = 5;
    failed = 0;
    collected = 0;
    game_running = 1;
    Step();
}

void JudgeWin() { game_running = (collected != 8); }
void JudgeFail() {
    for ( int i=0; i<14; i++ ) { bucket[i] = 0; }
    if ( !deals ) {
        for ( int i=0; i<SLOTS; i++ ) {
            int j = 0;
            while ( slots[i].max_successive[j] != 0 ) {
                bucket[slots[i].max_successive[j]%14]++;
                j++;
            }
        }
        for ( int i=1; i<=CARD_SET; i++ ) {
            if ( !bucket[i] ) { failed = true; }
        }
    }
}

void Step() {
    GetMaxSuccessive();
    EliminateCards();
    RevealHiddenCards();
    GetMaxSuccessive();
    JudgeFail();
    JudgeWin();
}

void DealCards() {
    for ( int i=0; i<SLOTS; i++ ) {
        if ( !slots[i].amount_revealed && !slots[i].amount_hidden ) {
            msg = "You can't deal cards when there are empty slots! :("; return;
        }
    }
    if ( !deals ) { msg = "No more cards to deal! :("; }
    else {
        for ( int i=0; i<SLOTS; i++ ) {
            slots[i].amount_revealed++;
            slots[i].revealed[slots[i].amount_revealed-1] = slots[i].heap[slots[i].amount_heap-1];
            slots[i].amount_heap--;
            // TODO - Animations
        }
        deals--; last_req = 'D';
        msg = "A new pack of cards dealt!";
    }
}

void MoveCards(int src, int des, int cnt) {
    if ( src == -1 || des == -1 ) { return; }
    if ( cnt > slots[src].movable ) { output = 3; }
    else {
        if ( (slots[des].revealed[slots[des].amount_revealed-1]%14 != slots[src].revealed[slots[src].amount_revealed-cnt]%14 + 1) &&
             ( (slots[des].amount_revealed + slots[des].amount_hidden) ) ) {
            msg = "You can't move cards there! :(";
        } else {
            for ( int i=0; i<cnt; i++ ) {
                slots[des].revealed[slots[des].amount_revealed+i] = slots[src].revealed[slots[src].amount_revealed-cnt+i];
            }
            slots[src].amount_revealed -= cnt;
            slots[des].amount_revealed += cnt;
            msg = "Successfully moved cards!";
            last_req = 'M';
            src_ = src;
            des_ = des;
            cnt_ = cnt;
        }
    }
}

void Regret() {
    switch ( last_req ) {
        case 'M': {
            if ( open_ ) {
                slots[src_].hidden[slots[src_].amount_hidden] = slots[src_].revealed[0];
                slots[src_].amount_hidden++;
                slots[src_].amount_revealed--;
            }
            for ( int i=0; i<cnt_; i++ ) {
                slots[src_].revealed[slots[src_].amount_revealed+i] = slots[des_].revealed[slots[des_].amount_revealed-cnt_+i];
            }
            slots[des_].amount_revealed -= cnt_;
            slots[src_].amount_revealed += cnt_;
            msg = "Revoked your last moving operation!";
            last_req = ' ';
            break;
        }
        case 'D': {
            for ( int i=0; i<SLOTS; i++ ) {
                slots[i].heap[slots[i].amount_heap] = slots[i].revealed[slots[i].amount_revealed-1];
                slots[i].amount_heap++;
                slots[i].amount_revealed--;
            }
            msg = "Revoked your last card-dealing operation!";
            deals++; last_req = ' ';
            break;
        }
        default: msg = "Unable to undo your last operation! :(";
    }
}

void GetHint() {
    int hint = 1, priority = 0;
    for ( int src=0; src<SLOTS; src++ ) {
        if ( !slots[src].amount_revealed ) { continue; }
        for ( int des=0; des<SLOTS; des++ ) {
            if ( src != des ) {
                if ( slots[des].ms_high%14 == 13 && slots[des].ms_high-slots[src].ms_low == 12 &&
                     slots[src].ms_high%14 >= slots[des].ms_low%14-1 ) {
                    priority = 1; srch = src; desh = des;
                    cnth = slots[des].ms_low%14-1;
                    goto finished;
                }
                if ( slots[src].ms_high == slots[des].ms_low - 1 && !priority) {
                    priority = 1; srch = src; desh = des;
                    cnth = slots[src].movable;
                    goto finished;
                }
            }
        }
    }
    finished:
    output = priority + 8;
    msg = "Hint: Hello World!";
    if ( !deals && output == 8 ) { msg = "No more hints! :("; }
}

#endif //PROJECT_SPIDER_H
