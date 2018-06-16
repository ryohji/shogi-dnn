#ifndef SHOGI_H_
#define SHOGI_H_

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
    K_HISYA, /* hisha */
    K_RYU, /* promoted hisha */
    K_GYOKU, /* gyokusho */
};

enum move {
    M_AGARU, /* go straight forward */
    M_NARU, /* go forward and promote */
    M_MIGI, /* go left ahead (from right) */
    M_MIGI_NARU, /* go left ahead (from right) and promote */
    M_HIDARI, /* go right ahead (from left) */
    M_HIDARI_NARU, /* go right ahead (from left) */
    M_MIGI_YORU, /* move left */
    M_MIGI_YORI_NARU, /* move left and promote */
    M_HIDARI_YORU, /* move right */
    M_HIDARI_YORI_NARU, /* move right and promote */
    M_MIGI_HIKU, /* back left */
    M_MIGI_HIKI_NARU, /* back left and promote */
    M_HIDARI_HIKU, /* back right */
    M_HIDARI_HIKI_NARU, /* back right and promote */
    M_HIKU, /* back straight */
    M_HIKI_NARU, /* back and promote */
    M_UTSU, /* put piece from hand */
};

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
