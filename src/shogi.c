#include "shogi.h"
#include "move.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void reorder_captured(struct board *b);

static int can_be_move_in(enum cell c);

struct board board_new() {
    const struct board board = { {
        CELL_W_KYO, CELL_W_KEI, CELL_W_GIN, CELL_W_KIN, CELL_W_GYOKU, CELL_W_KIN, CELL_W_GIN, CELL_W_KEI, CELL_W_KYO,
        CELL_BLANK, CELL_W_HISHA, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_W_KAKU, CELL_BLANK,
        CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU, CELL_W_FU,
        CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK,
        CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK,
        CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK,
        CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU, CELL_B_FU,
        CELL_BLANK, CELL_B_KAKU, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_BLANK, CELL_B_HISHA, CELL_BLANK,
        CELL_B_KYO, CELL_B_KEI, CELL_B_GIN, CELL_B_KIN, CELL_B_GYOKU, CELL_B_KIN, CELL_B_GIN, CELL_B_KEI, CELL_B_KYO,
    }, {
        CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK,
        CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK,
        CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK,
        CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK, CAPT_BLANK,
    }, };
    return board;
}

void board_init(struct board* board) {
    *board = board_new();
}

struct board_maybe board_apply(const struct board *original, int move_code) {
    const struct move move = move_describe(move_code);
    enum cell cell = original->cell[(8 - move.dan) * 9 + move.suji];
    if (can_be_move_in(cell)) {
        /* TODO */
    } else {
        struct board_maybe result = {*original, Nothing};
        return result;
    }
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
    int hand[14] = {0};
    char *p = buffer;
    int i;
    for (i = 0; i < NUMBER_OF_CAPTS; ++i) {
        switch (b->captured[i]) {
        case CAPT_W_FU: hand[0] += 1; break;
        case CAPT_W_KYO: hand[1] += 1; break;
        case CAPT_W_KEI: hand[2] += 1; break;
        case CAPT_W_GIN: hand[3] += 1; break;
        case CAPT_W_KIN: hand[4] += 1; break;
        case CAPT_W_KAKU: hand[5] += 1; break;
        case CAPT_W_HISHA: hand[6] += 1; break;
        case CAPT_B_FU: hand[7] += 1; break;
        case CAPT_B_KYO: hand[8] += 1; break;
        case CAPT_B_KEI: hand[9] += 1; break;
        case CAPT_B_GIN: hand[10] += 1; break;
        case CAPT_B_KIN: hand[11] += 1; break;
        case CAPT_B_KAKU: hand[12] += 1; break;
        case CAPT_B_HISHA: hand[13] += 1; break;
        default: break;
        }
    }
    p += sprintf(p, "c-%d a-%d 9-%d 7-%d 5-%d 3-%d 1-%02d\n",
        hand[6], hand[5], hand[4], hand[3], hand[2], hand[1], hand[0]);
    for (i = 0; i < NUMBER_OF_CELLS; ++i) {
        const char *const padding = i % 9 == 0 ? " " : "";
        const char separator = i % 9 != 8 ? ' ' : '\n';
        p += sprintf(p, "%s%s%c", padding, _str_cell(b->cell[i]), separator);
    }
    p += sprintf(p, "c-%d a-%d 9-%d 7-%d 5-%d 3-%d 1-%02d",
        hand[13], hand[12], hand[11], hand[10], hand[9], hand[8], hand[7]);
    return strdup(buffer);
}

/* static helper functions */
static int compar_captured(const void *a, const void *b);

void reorder_captured(struct board *b) {
    qsort(b->captured, NUMBER_OF_CAPTS, sizeof(enum captured), compar_captured);
}

int compar_captured(const void *a, const void *b) {
    return *(const enum captured*)a - *(const enum captured*)b;
}

int can_be_move_in(enum cell c) {
    switch (c) {
    case CELL_W_FU:
    case CELL_W_TO:
    case CELL_W_KYO:
    case CELL_W_NARIKYO:
    case CELL_W_KEI:
    case CELL_W_NARIKEI:
    case CELL_W_GIN:
    case CELL_W_NARIGIN:
    case CELL_W_KIN:
    case CELL_W_KAKU:
    case CELL_W_UMA:
    case CELL_W_HISHA:
    case CELL_W_RYU:
    case CELL_W_GYOKU:
        return 0;
    default:
        return 1;
    }
}

#ifdef UNITTEST_
int main() {
    struct board b;
    board_init(&b);
    char *str = board_tostring(&b);
    puts(str);
    free(str);
    return 0;
}
#endif
