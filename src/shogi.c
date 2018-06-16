#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shogi.h"

#define NUMBER_OF_CELLS 81
#define NUMBER_OF_HANDS 38

struct board {
    enum cell cell[NUMBER_OF_CELLS]; /* 99, 89, ..., 19, 98, 88, ...., 18, ..., 91, 81, ..., 11 */
    enum cell hand[NUMBER_OF_HANDS]; /* All shogi koma except two of Gyoku. Sorted */
};

struct board* board_new() {
    const enum cell init[NUMBER_OF_CELLS] = {
        CELL_W_KYO, CELL_W_KEI, CELL_W_GIN, CELL_W_KIN, CELL_W_GYOKU, CELL_W_KIN, CELL_W_GIN, CELL_W_KEI, CELL_W_KYO,
        CELL_BLANK, CELL_W_HISHA, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_W_KAKU, CELL_BLANK,
        CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU,
        CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK,
        CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK,
        CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK,
        CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU,
        CELL_BLANK, CELL_B_KAKU, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_B_HISHA, CELL_BLANK,
        CELL_B_KYO, CELL_B_KEI, CELL_B_GIN, CELL_B_KIN, CELL_B_GYOKU, CELL_B_KIN, CELL_B_GIN, CELL_B_KEI, CELL_B_KYO,
    };
    struct board *const b = malloc(sizeof(struct board));
    memcpy(b->cell, init, sizeof(init));
    memset(b->hand, CELL_BLANK, NUMBER_OF_HANDS);
    return b;
}

void board_delete(struct board *b) {
    free(b);
}

static const char* _str_cell(enum cell cell) {
    switch (cell) {
    case CELL_B_GYOKU: return "Be";
    case CELL_B_RYU: return "Bd";
    case CELL_B_HISHA: return "Bc";
    case CELL_B_UMA: return "Bb";
    case CELL_B_KAKU: return "Ba";
    case CELL_B_KIN: return "B9";
    case CELL_B_NARIGIN: return "B8";
    case CELL_B_GIN: return "B7";
    case CELL_B_NARIKEI: return "B6";
    case CELL_B_KEI: return "B5";
    case CELL_B_NARIKYO: return "B4";
    case CELL_B_KYO: return "B3";
    case CELL_B_TO: return "B2";
    case CELL_B_FU: return "B1";
    case CELL_BLANK: return ". ";
    case CELL_W_FU: return "W1";
    case CELL_W_TO: return "W2";
    case CELL_W_KYO: return "W3";
    case CELL_W_NARIKYO: return "W4";
    case CELL_W_KEI: return "W5";
    case CELL_W_NARIKEI: return "W6";
    case CELL_W_GIN: return "W7";
    case CELL_W_NARIGIN: return "W8";
    case CELL_W_KIN: return "W9";
    case CELL_W_KAKU: return "Wa";
    case CELL_W_UMA: return "Wb";
    case CELL_W_HISHA: return "Wc";
    case CELL_W_RYU: return "Wd";
    case CELL_W_GYOKU: return "We";
    }
}

char* board_tostring(const struct board* b) {
    char buffer[1024];
    char *p = buffer;
    int i;
    p += sprintf(p, "W:c-0 a-0 7-0 5-0 3-0 1-00\n");
    while (i < NUMBER_OF_CELLS) {
        p += sprintf(p, "%s%c", _str_cell(b->cell[i]), (i % 9 != 8 ? ' ' : '\n'));
        ++i;
    }
    p += sprintf(p, "B:c-0 a-0 7-0 5-0 3-0 1-00");
    return strdup(buffer);
}

#ifdef UNITTEST_
int main() {
    struct board *const b = board_new();
    char *str = board_tostring(b);
    puts(str);
    free(str);
    board_delete(b);
    return 0;
}
#endif
