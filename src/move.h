#ifndef MOVE_H_
#define MOVE_H_

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

enum suji {
    SUJI_9,
    SUJI_8,
    SUJI_7,
    SUJI_6,
    SUJI_5,
    SUJI_4,
    SUJI_3,
    SUJI_2,
    SUJI_1,
};

enum dan {
    DAN_9,
    DAN_8,
    DAN_7,
    DAN_6,
    DAN_5,
    DAN_4,
    DAN_3,
    DAN_2,
    DAN_1,
};

#define NUMBER_OF_MOVES 5530

/**
 * SUJI                     DAN
 * 9  8  7  6  5  4  3  2  1
 *                           9
 *                           8
 *                           7
 *                           6
 *                           5
 *          * <== 64         4
 *              SUJI-DAN     3
 *                           2
 *                           1
 */
struct move {
    enum suji suji;
    enum dan dan;
    enum koma koma;
    enum act act;
};

struct move move_describe(unsigned code);

#endif
