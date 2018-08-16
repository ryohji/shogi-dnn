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

/**
 * Maintain the states of Shogi board.
 */
struct board;

/**
 * Allocate and initialize Shogi board.
 */
struct board* board_new();

/**
 * Free the board.
 */
void board_delete(struct board*);

/**
 * Acquire string representation of the board.
 * 
 * you must call free() for returned string.
 */
char* board_tostring(const struct board*);

#endif
