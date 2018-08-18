#include "shogi.h"
#include "move.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static enum cell* cell_targeted(struct board* board, const struct move* move);
/**
 * Replace one of `target` by `update` and return non-zero if `target` in the `board`,
 * otherwise the `board` untouched and return zero.
 */
static int captured_update(struct board* board, enum captured target, enum captured update);
static int dup_Fu(const struct move* move, const struct board* board);
static int uchi_Fu_zume(const struct move* move, const struct board* board);
static int release_matching(const struct move* move, struct board* board);
static enum cell koma_to_cell(enum koma koma);
static int move_matching(const struct move* move, struct board* board);
static int can_be_move_in(enum cell c);
static enum captured cell_to_captured(enum cell cell);

static void diagonal_swap(void *array, unsigned nelem, unsigned width);
static void array_apply(void *array, unsigned nelem, unsigned width, void (*f)(void *));
static void cell_invert(void* c);
static void captured_invert(void* c);

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
    struct board_maybe result = {*original, Nothing};
    const struct move move = move_describe(move_code);
    enum cell* const cell = cell_targeted(&result.board, &move);
    if (move.act == A_UTSU && *cell == CELL_BLANK
        && (move.koma != K_FU || !(dup_Fu(&move, original) || uchi_Fu_zume(&move, original)))
        && release_matching(&move, &result.board)) { /* from captured */
        *cell = koma_to_cell(move.koma);
        result.maybe = Just;
    } else if (move.act != A_UTSU && can_be_move_in(*cell)
        && move_matching(&move, &result.board)) { /* from board */
        result.maybe = Just;
    } else {
        /* There is no move. */
    }
    return result;
}

struct board board_invert(const struct board *original) {
    struct board board = *original; /* make copy */
    diagonal_swap(board.cell, NUMBER_OF_CELLS, sizeof(enum cell));
    array_apply(board.cell, NUMBER_OF_CELLS, sizeof(enum cell), cell_invert);
    diagonal_swap(board.captured, NUMBER_OF_CAPTS, sizeof(enum captured));
    array_apply(board.captured, NUMBER_OF_CAPTS, sizeof(enum captured), captured_invert);
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
static int compar_captured(const void *a, const void *b) {
    return *(const enum captured*)a - *(const enum captured*)b;
}

enum cell* cell_targeted(struct board* board, const struct move* move) {
    return board->cell + (8 - move->dan) * 9 + (8 - move->suji);
}

int captured_update(struct board* board, enum captured target, enum captured update) {
    const size_t width = sizeof(enum captured);
    enum captured* const array = board->captured;
    enum captured* const pos = bsearch(&target, array, NUMBER_OF_CAPTS, width, compar_captured);
    if (pos) {
        *pos = update;
        qsort(array, NUMBER_OF_CAPTS, width, compar_captured);
        return !0;
    } else {
        return 0;
    }
}

int dup_Fu(const struct move* move, const struct board* board) {
    const enum cell* it = board->cell + move->suji;
    const enum cell* const end = it + NUMBER_OF_CELLS;
    while (it != end && *it != CELL_B_FU) {
        it += 9;
    }
    return it != end;
}

int uchi_Fu_zume(const struct move* move, const struct board* board) {
    /* TODO */
    return 0;
}

static enum captured koma_to_captured(enum koma koma);

int release_matching(const struct move *move, struct board* board) {
    return captured_update(board, koma_to_captured(move->koma), CAPT_BLANK);
}

enum captured koma_to_captured(enum koma koma) {
    switch (koma) {
    case K_FU:    return CAPT_B_FU;
    case K_KYOU:  return CAPT_B_KYO;
    case K_KEI:   return CAPT_B_KEI;
    case K_GIN:   return CAPT_B_GIN;
    case K_KIN:   return CAPT_B_KIN;
    case K_KAKU:  return CAPT_B_KAKU;
    case K_HISHA: return CAPT_B_HISHA;
    default:      return (enum captured)-1;
    }
}

enum cell koma_to_cell(enum koma koma) {
    switch (koma) {
    case K_FU:       return CELL_B_FU;
    case K_TO:       return CELL_B_TO;
    case K_KYOU:     return CELL_B_KYO;
    case K_NARIKYOU: return CELL_B_NARIKYO;
    case K_KEI:      return CELL_B_KEI;
    case K_NARIKEI:  return CELL_B_NARIKEI;
    case K_GIN:      return CELL_B_GIN;
    case K_NARIGIN:  return CELL_B_NARIGIN;
    case K_KIN:      return CELL_B_KIN;
    case K_KAKU:     return CELL_B_KAKU;
    case K_UMA:      return CELL_B_UMA;
    case K_HISHA:    return CELL_B_HISHA;
    case K_RYU:      return CELL_B_RYU;
    case K_GYOKU:    return CELL_B_GYOKU;
    }
}

static enum cell* koma_origin(const struct move* move, enum cell* board);
static enum koma with_promotion(enum koma koma, enum act act);

int move_matching(const struct move* move, struct board* board) {
    enum cell* const origin = koma_origin(move, board->cell);
    if (origin != board->cell + NUMBER_OF_CELLS) {
        enum cell* const target = cell_targeted(board, move);
        (void) captured_update(board, CAPT_BLANK, cell_to_captured(*target));
        *target = koma_to_cell(with_promotion(move->koma, move->act));
        *origin = CELL_BLANK;
        return !0;
    } else {
        return 0;
    }
}

struct cells {
    unsigned n;
    enum cell* cell[8];
};

/**
 * Choose cell pointers thet possibly the origin of move.
 */
static struct cells cells_from(const struct move* move, enum cell* cells);

enum cell* koma_origin(const struct move* move, enum cell* board) {
    const struct cells cells = cells_from(move, board);
    unsigned n = 0;
    while (n < cells.n && *cells.cell[n] == CELL_BLANK) { /* Skip blank cells */
        n += 1;
    }
    if (n == cells.n || *cells.cell[n] != koma_to_cell(move->koma)) {
        return board + NUMBER_OF_CELLS; /* NOT FOUND; all trails filled with blank, or first found koma does not match. */
    } else {
        return cells.cell[n]; /* TA-DA! */
    }
}

struct cells cells_from(const struct move* move, enum cell* cells) {
    struct cells value = {0};
    unsigned d = move->dan;
    unsigned s = move->suji;

#define CELL_PTR(dan, suji) (cells + (8 - dan) * 9 + (8 - suji))

    switch (move->act) {
    case A_AGARU:
    case A_NARU:
        switch (move->koma) {
        case K_KYOU:
        case K_HISHA:
        case K_RYU:
            while (d--) {
                value.cell[value.n++] = CELL_PTR(d, s);
            }
            break;
        default:
            value.n = 1;
            value.cell[0] = CELL_PTR(d - 1, s);
            break;
        }
        return value;
    /* TODO */
    default:
        return value;
    }

#undef CELL_PTR
}

static int promoting(enum act act);

enum koma with_promotion(enum koma koma, enum act act) {
    if (promoting(act)) {
        switch (koma) {
        case K_FU:      return K_TO;
        case K_KYOU:    return K_NARIKYOU;
        case K_KEI:     return K_NARIKEI;
        case K_GIN:     return K_NARIGIN;
        case K_KAKU:    return K_UMA;
        case K_HISHA:   return K_RYU;
        default:
            fprintf(stderr, "Invalid promotion: koma=%d act=%d\n", koma, act);
            return koma;
        }
    } else {
        return koma;
    }
}

int promoting(enum act act) {
    switch (act) {
    case A_NARU:
    case A_MIGI_NARU:
    case A_HIDARI_NARU:
    case A_MIGI_YORI_NARU:
    case A_HIDARI_YORI_NARU:
    case A_MIGI_HIKI_NARU:
    case A_HIDARI_HIKI_NARU:
    case A_HIKI_NARU:
        return !0;
    default:
        return 0;
    }
}

int can_be_move_in(enum cell c) {
    switch (c) {
    case CELL_B_FU:
    case CELL_B_TO:
    case CELL_B_KYO:
    case CELL_B_NARIKYO:
    case CELL_B_KEI:
    case CELL_B_NARIKEI:
    case CELL_B_GIN:
    case CELL_B_NARIGIN:
    case CELL_B_KIN:
    case CELL_B_KAKU:
    case CELL_B_UMA:
    case CELL_B_HISHA:
    case CELL_B_RYU:
    case CELL_B_GYOKU:
        return 0;
    default:
        return !0;
    }
}

enum captured cell_to_captured(enum cell cell) {
    switch (cell) {
    default:
        return CAPT_BLANK;
    case CELL_W_FU:
    case CELL_W_TO:
        return CAPT_B_FU;
    case CELL_W_KYO:
    case CELL_W_NARIKYO:
        return CAPT_B_KYO;
    case CELL_W_KEI:
    case CELL_W_NARIKEI:
        return CAPT_B_KEI;
    case CELL_W_GIN:
    case CELL_W_NARIGIN:
        return CAPT_B_GIN;
    case CELL_W_KIN:
        return CAPT_B_KIN;
    case CELL_W_KAKU:
    case CELL_W_UMA:
        return CAPT_B_KAKU;
    case CELL_W_HISHA:
    case CELL_W_RYU:
        return CAPT_B_HISHA;
    }
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

static enum cell inverted_cell(enum cell c) {
    switch (c) {
    case CELL_B_GYOKU:   return CELL_W_GYOKU;
    case CELL_B_RYU:     return CELL_W_RYU;
    case CELL_B_HISHA:   return CELL_W_HISHA;
    case CELL_B_UMA:     return CELL_W_UMA;
    case CELL_B_KAKU:    return CELL_W_KAKU;
    case CELL_B_KIN:     return CELL_W_KIN;
    case CELL_B_NARIGIN: return CELL_W_NARIGIN;
    case CELL_B_GIN:     return CELL_W_GIN;
    case CELL_B_NARIKEI: return CELL_W_NARIKEI;
    case CELL_B_KEI:     return CELL_W_KEI;
    case CELL_B_NARIKYO: return CELL_W_NARIKYO;
    case CELL_B_KYO:     return CELL_W_KYO;
    case CELL_B_TO:      return CELL_W_TO;
    case CELL_B_FU:      return CELL_W_FU;
    case CELL_BLANK:     return CELL_BLANK;
    case CELL_W_FU:      return CELL_B_FU;
    case CELL_W_TO:      return CELL_B_TO;
    case CELL_W_KYO:     return CELL_B_KYO;
    case CELL_W_NARIKYO: return CELL_B_NARIKYO;
    case CELL_W_KEI:     return CELL_B_KEI;
    case CELL_W_NARIKEI: return CELL_B_NARIKEI;
    case CELL_W_GIN:     return CELL_B_GIN;
    case CELL_W_NARIGIN: return CELL_B_NARIGIN;
    case CELL_W_KIN:     return CELL_B_KIN;
    case CELL_W_KAKU:    return CELL_B_KAKU;
    case CELL_W_UMA:     return CELL_B_UMA;
    case CELL_W_HISHA:   return CELL_B_HISHA;
    case CELL_W_RYU:     return CELL_B_RYU;
    case CELL_W_GYOKU:   return CELL_B_GYOKU;
    }
}

void cell_invert(void* p) {
    enum cell *const c = p;
    *c = inverted_cell(*c);
}

static enum captured inverted_captured(enum captured c) {
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

void captured_invert(void *p) {
    enum captured *const c = p;
    *c = inverted_captured(*c);
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
