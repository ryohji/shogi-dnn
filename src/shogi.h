#ifndef SHOGI_H_
#define SHOGI_H_

#define NUMBER_OF_KOMA 14
enum koma {
    K_FU, /* fuhyo */
    K_TO, /* promoted fuhyo */
    K_KYOU, /* kyousha */
    K_NARIKYOU, /* promoted kyosha */
    K_KEI, /* keima */
    K_NARIKEI, /* promoted keima */
    K_GIN, /* ginsho */
    K_NARIGIN, /* promoted ginsho */
    K_KIN, /* kinsho */
    K_KAKU, /* kakugyo */
    K_UMA, /* promoted kakugyo */
    K_HISHA, /* hisha */
    K_RYU, /* promoted hisha */
    K_GYOKU, /* gyokusho */
};

#define NUMBER_OF_ACTS 17
enum act {
    A_AGARU, /* go straight forward */
    A_NARU, /* go forward and promote */
    A_MIGI, /* go left ahead (from right) */
    A_MIGI_NARU, /* go left ahead (from right) and promote */
    A_HIDARI, /* go right ahead (from left) */
    A_HIDARI_NARU, /* go right ahead (from left) */
    A_MIGI_YORU, /* move left */
    A_MIGI_YORI_NARU, /* move left and promote */
    A_HIDARI_YORU, /* move right */
    A_HIDARI_YORI_NARU, /* move right and promote */
    A_MIGI_HIKU, /* back left */
    A_MIGI_HIKI_NARU, /* back left and promote */
    A_HIDARI_HIKU, /* back right */
    A_HIDARI_HIKI_NARU, /* back right and promote */
    A_HIKU, /* back straight */
    A_HIKI_NARU, /* back and promote */
    A_UTSU, /* put piece from hand */
};

#define NUMBER_OF_COLUMNS 9
#define NUMBER_OF_ROWS 9

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
