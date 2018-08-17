#include "shogi.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void reorder_captured(struct board *b);

static void diagonal_swap(void *array, unsigned nelem, unsigned width);
static void array_apply(void *array, unsigned nelem, unsigned width, void (*f)(void *));
static void cell_invert(void* c);
static enum captured inverted_captured(enum captured c);

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

struct board board_invert(const struct board *original) {
    struct board board = *original;
    unsigned i;
    diagonal_swap(board.cell, NUMBER_OF_CELLS, sizeof(enum cell));
    array_apply(board.cell, NUMBER_OF_CELLS, sizeof(enum cell), cell_invert);
    diagonal_swap(board.captured, NUMBER_OF_CAPTS, sizeof(enum captured));
    for (i = 0; i < NUMBER_OF_CAPTS; ++i) {
        board.captured[i] = inverted_captured(board.captured[i]);
    }
    return board;
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

void diagonal_swap(void *array, unsigned nelem, unsigned width) {
    char *const t = alloca(width);
    char *p = array;
    char *q = p + (nelem - 1) * width;
    while (p < q) {
        memcpy(t, p, width);
        memcpy(p, q, width);
        memcpy(q, t, width);
        p += width;
        q -= width;
    }
}

void array_apply(void *array, unsigned nelem, unsigned width, void (*f)(void *)) {
    char *p = array;
    char *q = p + nelem * width;
    while (p != q) {
        f(p);
        p += width;
    }
}

void cell_invert(void* p) {
    enum cell *c = p;
    switch (*c) {
    case CELL_B_GYOKU:   *c = CELL_W_GYOKU;   break;
    case CELL_B_RYU:     *c = CELL_W_RYU;     break;
    case CELL_B_HISHA:   *c = CELL_W_HISHA;   break;
    case CELL_B_UMA:     *c = CELL_W_UMA;     break;
    case CELL_B_KAKU:    *c = CELL_W_KAKU;    break;
    case CELL_B_KIN:     *c = CELL_W_KIN;     break;
    case CELL_B_NARIGIN: *c = CELL_W_NARIGIN; break;
    case CELL_B_GIN:     *c = CELL_W_GIN;     break;
    case CELL_B_NARIKEI: *c = CELL_W_NARIKEI; break;
    case CELL_B_KEI:     *c = CELL_W_KEI;     break;
    case CELL_B_NARIKYO: *c = CELL_W_NARIKYO; break;
    case CELL_B_KYO:     *c = CELL_W_KYO;     break;
    case CELL_B_TO:      *c = CELL_W_TO;      break;
    case CELL_B_FU:      *c = CELL_W_FU;      break;
    case CELL_BLANK:     *c = CELL_BLANK;     break;
    case CELL_W_FU:      *c = CELL_B_FU;      break;
    case CELL_W_TO:      *c = CELL_B_TO;      break;
    case CELL_W_KYO:     *c = CELL_B_KYO;     break;
    case CELL_W_NARIKYO: *c = CELL_B_NARIKYO; break;
    case CELL_W_KEI:     *c = CELL_B_KEI;     break;
    case CELL_W_NARIKEI: *c = CELL_B_NARIKEI; break;
    case CELL_W_GIN:     *c = CELL_B_GIN;     break;
    case CELL_W_NARIGIN: *c = CELL_B_NARIGIN; break;
    case CELL_W_KIN:     *c = CELL_B_KIN;     break;
    case CELL_W_KAKU:    *c = CELL_B_KAKU;    break;
    case CELL_W_UMA:     *c = CELL_B_UMA;     break;
    case CELL_W_HISHA:   *c = CELL_B_HISHA;   break;
    case CELL_W_RYU:     *c = CELL_B_RYU;     break;
    case CELL_W_GYOKU:   *c = CELL_B_GYOKU;   break;
    }
}

enum captured inverted_captured(enum captured c) {
    switch (c) {
    case CAPT_B_HISHA: return CAPT_W_HISHA;
    case CAPT_B_KAKU:  return CAPT_W_KAKU;
    case CAPT_B_KIN:   return CAPT_W_KIN;
    case CAPT_B_GIN:   return CAPT_W_GIN;
    case CAPT_B_KEI:   return CAPT_W_KEI;
    case CAPT_B_KYO:   return CAPT_W_KYO;
    case CAPT_B_FU:    return CAPT_W_FU;
    case CAPT_BLANK:   return CAPT_BLANK;
    case CAPT_W_FU:    return CAPT_B_FU;
    case CAPT_W_KYO:   return CAPT_B_KYO;
    case CAPT_W_KEI:   return CAPT_B_KEI;
    case CAPT_W_GIN:   return CAPT_B_GIN;
    case CAPT_W_KIN:   return CAPT_B_KIN;
    case CAPT_W_KAKU:  return CAPT_B_KAKU;
    case CAPT_W_HISHA: return CAPT_B_HISHA;
    }
}

#ifdef UNITTEST_
int main() {
    struct board b;
    board_init(&b);
    char *str = board_tostring(&b);
    puts(str);
    free(str);

    b.cell[18] = CELL_BLANK;
    b.cell[27] = CELL_W_FU;
    b = board_invert(&b);
    str = board_tostring(&b);
    puts(str);
    free(str);

    return 0;
}
#endif
