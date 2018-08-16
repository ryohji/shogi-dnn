#ifndef SHOGI_H_
#define SHOGI_H_

/**
 * States of Shogi cell.
 */
enum cell {
    CELL_B_GYOKU,
    CELL_B_RYU,
    CELL_B_HISHA,
    CELL_B_UMA,
    CELL_B_KAKU,
    CELL_B_KIN,
    CELL_B_NARIGIN,
    CELL_B_GIN,
    CELL_B_NARIKEI,
    CELL_B_KEI,
    CELL_B_NARIKYO,
    CELL_B_KYO,
    CELL_B_TO,
    CELL_B_FU,
    CELL_BLANK,
    CELL_W_FU,
    CELL_W_TO,
    CELL_W_KYO,
    CELL_W_NARIKYO,
    CELL_W_KEI,
    CELL_W_NARIKEI,
    CELL_W_GIN,
    CELL_W_NARIGIN,
    CELL_W_KIN,
    CELL_W_KAKU,
    CELL_W_UMA,
    CELL_W_HISHA,
    CELL_W_RYU,
    CELL_W_GYOKU,
};

/**
 * States of Shogi captives.
 */
enum captured {
    CAPT_B_HISHA,
    CAPT_B_KAKU,
    CAPT_B_KIN,
    CAPT_B_GIN,
    CAPT_B_KEI,
    CAPT_B_KYO,
    CAPT_B_FU,
    CAPT_BLANK,
    CAPT_W_FU,
    CAPT_W_KYO,
    CAPT_W_KEI,
    CAPT_W_GIN,
    CAPT_W_KIN,
    CAPT_W_KAKU,
    CAPT_W_HISHA,
};

enum player {
    BLACK, /* sen-te; the player who plays first */
    WHITE, /* go-te */
};

#define NUMBER_OF_CELLS (9 * 9)
#define NUMBER_OF_CAPTS ((9 + 2 + (9 - 1)) * 2) /* except for 2 gyoku */

/**
 * Maintain the states of Shogi board.
 */
struct board {
    enum cell cell[NUMBER_OF_CELLS]; /* 99, 89, ..., 19, 98, 88, ...., 18, ..., 91, 81, ..., 11 */
    enum captured captured[NUMBER_OF_CAPTS]; /* All shogi koma except two of Gyoku. Sorted */
};

enum maybe {
    Just,
    Nothing,
};

struct board_maybe {
    struct board board;
    enum maybe maybe;
};

/**
 * Acquire initialized copy of Shogi board.
 */
struct board board_new();

/**
 * (Re-)Initialize given Shogi board.
 */
void board_init(struct board *board);

/**
 * Get the board, applied given move.
 * 
 * If given move can not be applied to the given board, maybe field of the returned board_maybe set Nothing.
 * Otherwise set Just and board structure represents the board applied given move.
 */
struct board_maybe board_apply(const struct board *board, int move_code);

/**
 * Generate the board, the players inverted.
 */
struct board board_invert(const struct board *board);

/**
 * Acquire string representation of the board.
 * 
 * you must call free() for returned string.
 */
char* board_tostring(const struct board*);

#endif
