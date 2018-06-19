#include "shogi.h"

#include <stdint.h>
#include <stdlib.h>

struct move_detail {
    int8_t dan;
    int8_t suji;
    enum koma koma;
    enum move move;
};

struct move_detail move_detail(unsigned move) {
    struct move_detail result;
    div_t d = {move};
    d = div(d.quot, NUMBER_OF_MOVES);
    result.move = d.rem;
    d = div(d.quot, NUMBER_OF_ROWS);
    result.dan = d.rem;
    d = div(d.quot, NUMBER_OF_COLUMNS);
    result.suji = d.rem;
    d = div(d.quot, NUMBER_OF_KOMA);
    result.koma = d.rem;
    return result;
}

#ifdef UNITTEST_
#include <stdio.h>

#include "filter.h"

static const char* _str_koma(unsigned koma) {
    switch (koma) {
    case 0: return "FU";
    case 1: return "TO";
    case 2: return "KYOU";
    case 3: return "NARI-KYOU";
    case 4: return "KEI";
    case 5: return "NARI-KEI";
    case 6: return "GIN";
    case 7: return "NARI-GIN";
    case 8: return "KIN";
    case 9: return "KAKU";
    case 10: return "NARI-KAKU";
    case 11: return "HISHA";
    case 12: return "NARI-HISHA";
    case 13: return "GYOKU";
    default: return "";
    }
}

static const char* _str_move(unsigned move) {
    switch (move) {
    case 0: return "AGARU";
    case 1: return "NARU";
    case 2: return "MIGI";
    case 3: return "MIGI_NARU";
    case 4: return "HIDARI";
    case 5: return "HIDARI_NARU";
    case 6: return "MIGI_YORU";
    case 7: return "MIGI_YORI_NARU";
    case 8: return "HIDARI_YORU";
    case 9: return "HIDARI_YORI_NARU";
    case 10: return "MIGI_HIKU";
    case 11: return "MIGI_HIKI_NARU";
    case 12: return "HIDARI_HIKU";
    case 13: return "HIDARI_HIKI_NARU";
    case 14: return "HIKU";
    case 15: return "HIKI_NARU";
    case 16: return "UTSU";
    default: return "";
    }
}

static unsigned remove_improper_promotion(const void *elem, void *context) {
    const struct move_detail move = move_detail(*(uint16_t *)elem);
    return (move.dan > 2 || move.koma == K_TO || move.koma == K_NARIKYOU || move.koma == K_NARIKEI
    || move.koma == K_NARIGIN || move.koma == K_UMA || move.koma == K_RYU || move.koma == K_GYOKU)
    && (move.move == M_NARU || move.move == M_MIGI_NARU || move.move == M_HIDARI_NARU
    || move.move == M_MIGI_YORI_NARU || move.move == M_HIDARI_YORI_NARU
    || move.move == M_MIGI_HIKI_NARU || move.move == M_HIDARI_HIKI_NARU || move.move == M_HIKI_NARU);
}

static unsigned remove_forbidden_move(const void *elem, void *context) {
    const struct move_detail move = move_detail(*(uint16_t *)elem);
    switch (move.koma) {
    case K_FU:
    case K_KYOU:
        return move.move != M_AGARU && move.move != M_NARU;
    case K_KEI:
        return move.move != M_MIGI && move.move != M_MIGI_NARU
        && move.move != M_HIDARI && move.move != M_HIDARI_NARU;
    case K_GIN:
        return move.move != M_AGARU && move.move != M_NARU
        && move.move != M_MIGI && move.move != M_MIGI_NARU
        && move.move != M_HIDARI && move.move != M_HIDARI_NARU
        && move.move != M_MIGI_HIKU && move.move != M_MIGI_HIKI_NARU
        && move.move != M_HIDARI_HIKU && move.move != M_HIDARI_HIKI_NARU;
    case K_KIN:
    case K_TO:
    case K_NARIKYOU:
    case K_NARIKEI:
        return move.move != M_AGARU
        && move.move != M_MIGI
        && move.move != M_HIDARI
        && move.move != M_MIGI_YORU
        && move.move != M_HIDARI_YORU
        && move.move != M_HIKU;
    case K_KAKU:
        return move.move != M_MIGI && move.move != M_MIGI_NARU
        && move.move != M_HIDARI && move.move != M_HIDARI_NARU
        && move.move != M_MIGI_HIKU && move.move != M_MIGI_HIKI_NARU
        && move.move != M_HIDARI_HIKU && move.move != M_HIDARI_HIKI_NARU;
    case K_HISYA:
        return move.move != M_AGARU && move.move != M_NARU
        && move.move != M_MIGI_YORU && move.move != M_MIGI_YORI_NARU
        && move.move != M_HIDARI_YORU && move.move != M_HIDARI_YORI_NARU
        && move.move != M_HIKU && move.move != M_HIKI_NARU;
    default:
        return 0;
    }
}

static unsigned remove_enables_no_more_move(const void *elem, void *context) {
    const struct move_detail move = move_detail(*(uint16_t *)elem);
    switch (move.koma) {
    case K_FU:
    case K_KYOU:
        return move.dan == 0 && (move.move == M_AGARU || move.move == M_UTSU);
    case K_KEI:
        return (move.dan == 0 || move.dan == 1) && (move.move == M_MIGI || move.move == M_HIDARI || move.move == M_UTSU);
    default:
        return 0;
    }
}

static unsigned remove_gyoku_utsu(const void *elem, void *context) {
    const struct move_detail move = move_detail(*(uint16_t *)elem);
    return move.koma == K_GYOKU && move.move == M_UTSU;
}

int main() {
    const unsigned int N = NUMBER_OF_COLUMNS * NUMBER_OF_ROWS * NUMBER_OF_KOMA * NUMBER_OF_MOVES;
    uint16_t *const all_moves = malloc(sizeof(uint16_t) * N);

    uint16_t *it, *end;
    end = all_moves + N;
    for (it = all_moves; it != end; ++it) {
        *it = it - all_moves;
    }

    end = filter(all_moves, end, sizeof(uint16_t), remove_improper_promotion, NULL, all_moves);
    end = filter(all_moves, end, sizeof(uint16_t), remove_forbidden_move, NULL, all_moves);
    /* TODO remove moves from outside */
    end = filter(all_moves, end, sizeof(uint16_t), remove_enables_no_more_move, NULL, all_moves);
    end = filter(all_moves, end, sizeof(uint16_t), remove_gyoku_utsu, NULL, all_moves);

    /* dump result */
    for (it = all_moves; it != end; ++it) {
        const intptr_t i = it - all_moves;
        struct move_detail move = move_detail(*it);

        printf("[%5ld] %d %d %s %s\n", i, move.suji + 1, move.dan + 1, _str_koma(move.koma), _str_move(move.move));
    }
    free(all_moves);
    return 0;
}
#endif
