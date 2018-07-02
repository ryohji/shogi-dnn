#include "move.h"

#include <stdint.h>
#include <stdlib.h>

struct move_detail {
    int8_t dan;
    int8_t suji;
    enum koma koma;
    enum act move;
};

struct move_detail move_detail(unsigned move) {
    struct move_detail result;
    div_t d = {move};
    d = div(d.quot, NUMBER_OF_ACTS);
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
    case 3: return "NARIKYOU";
    case 4: return "KEI";
    case 5: return "NARIKEI";
    case 6: return "GIN";
    case 7: return "NARIGIN";
    case 8: return "KIN";
    case 9: return "KAKU";
    case 10: return "UMA";
    case 11: return "HISHA";
    case 12: return "RYU";
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
    && (move.move == A_NARU || move.move == A_MIGI_NARU || move.move == A_HIDARI_NARU
    || move.move == A_MIGI_YORI_NARU || move.move == A_HIDARI_YORI_NARU
    || move.move == A_MIGI_HIKI_NARU || move.move == A_HIDARI_HIKI_NARU || move.move == A_HIKI_NARU);
}

static unsigned remove_forbidden_move(const void *elem, void *context) {
    const struct move_detail move = move_detail(*(uint16_t *)elem);
    switch (move.koma) {
    case K_FU:
    case K_KYOU:
        return move.move != A_AGARU && move.move != A_NARU
        && move.move != A_UTSU;
    case K_KEI:
        return move.move != A_MIGI && move.move != A_MIGI_NARU
        && move.move != A_HIDARI && move.move != A_HIDARI_NARU
        && move.move != A_UTSU;
    case K_GIN:
        return move.move != A_AGARU && move.move != A_NARU
        && move.move != A_MIGI && move.move != A_MIGI_NARU
        && move.move != A_HIDARI && move.move != A_HIDARI_NARU
        && move.move != A_MIGI_HIKU && move.move != A_MIGI_HIKI_NARU
        && move.move != A_HIDARI_HIKU && move.move != A_HIDARI_HIKI_NARU
        && move.move != A_UTSU;
    case K_KIN:
    case K_TO:
    case K_NARIKYOU:
    case K_NARIKEI:
    case K_NARIGIN:
        return move.move != A_AGARU
        && move.move != A_MIGI
        && move.move != A_HIDARI
        && move.move != A_MIGI_YORU
        && move.move != A_HIDARI_YORU
        && move.move != A_HIKU
        && (move.koma != K_KIN || move.move != A_UTSU);
    case K_KAKU:
        return move.move != A_MIGI && move.move != A_MIGI_NARU
        && move.move != A_HIDARI && move.move != A_HIDARI_NARU
        && move.move != A_MIGI_HIKU && move.move != A_MIGI_HIKI_NARU
        && move.move != A_HIDARI_HIKU && move.move != A_HIDARI_HIKI_NARU
        && move.move != A_UTSU;
    case K_HISHA:
        return move.move != A_AGARU && move.move != A_NARU
        && move.move != A_MIGI_YORU && move.move != A_MIGI_YORI_NARU
        && move.move != A_HIDARI_YORU && move.move != A_HIDARI_YORI_NARU
        && move.move != A_HIKU && move.move != A_HIKI_NARU
        && move.move != A_UTSU;
    case K_UMA:
    case K_RYU:
    case K_GYOKU:
        return move.move == A_UTSU;
    }
}

static unsigned remove_enter_outside(const void *elem, void *context) {
    const struct move_detail move = move_detail(*(uint16_t *)elem);
    unsigned result = 0;
    if (move.dan == 0) {
        result |= move.move == A_MIGI_HIKU || move.move == A_MIGI_HIKI_NARU
        || move.move == A_HIDARI_HIKU || move.move == A_HIDARI_HIKI_NARU
        || move.move == A_HIKU || move.move == A_HIKI_NARU;
    } else if (move.dan == 8) {
        result |= move.move == A_AGARU || move.move == A_NARU
        || move.move == A_MIGI || move.move == A_MIGI_NARU
        || move.move == A_HIDARI || move.move == A_HIDARI_NARU;
    } else if (move.dan == 7 && move.koma == K_KEI) {
        result |= move.move == A_HIDARI || move.move == A_MIGI;
    }
    if (move.suji == 0) {
        result |= move.move == A_MIGI || move.move == A_MIGI_NARU
        || move.move == A_MIGI_YORU || move.move == A_MIGI_YORI_NARU
        || move.move == A_MIGI_HIKU || move.move == A_MIGI_HIKI_NARU;
    } else if (move.suji == 8) {
        result |= move.move == A_HIDARI || move.move == A_HIDARI_NARU
        || move.move == A_HIDARI_YORU || move.move == A_HIDARI_YORI_NARU
        || move.move == A_HIDARI_HIKU || move.move == A_HIDARI_HIKI_NARU;
    }
    return result;
}

static unsigned remove_enables_no_more_move(const void *elem, void *context) {
    const struct move_detail move = move_detail(*(uint16_t *)elem);
    switch (move.koma) {
    case K_FU:
    case K_KYOU:
        return move.dan == 0 && (move.move == A_AGARU || move.move == A_UTSU);
    case K_KEI:
        return (move.dan == 0 || move.dan == 1) && (move.move == A_MIGI || move.move == A_HIDARI || move.move == A_UTSU);
    default:
        return 0;
    }
}

int main() {
    const unsigned int N = NUMBER_OF_COLUMNS * NUMBER_OF_ROWS * NUMBER_OF_KOMA * NUMBER_OF_ACTS;
    uint16_t *const all_moves = malloc(sizeof(uint16_t) * N);

    uint16_t *it, *end;
    end = all_moves + N;
    for (it = all_moves; it != end; ++it) {
        *it = it - all_moves;
    }

    end = filter(all_moves, end, sizeof(uint16_t), remove_improper_promotion, NULL, all_moves);
    end = filter(all_moves, end, sizeof(uint16_t), remove_forbidden_move, NULL, all_moves);
    end = filter(all_moves, end, sizeof(uint16_t), remove_enter_outside, NULL, all_moves);
    end = filter(all_moves, end, sizeof(uint16_t), remove_enables_no_more_move, NULL, all_moves);

    /* dump result */
    for (it = all_moves; it != end; ++it) {
        const intptr_t i = it - all_moves;
        struct move_detail move = move_detail(*it);

        printf("  MOVE_%d%d%s%s%s,\n", move.suji + 1, move.dan + 1, _str_koma(move.koma),
        move.move ? "_" : "", move.move ? _str_move(move.move) : "");
    }
    free(all_moves);
/*
    {
        uint16_t moves[] = {((K_GIN * NUMBER_OF_COLUMNS + 0) * NUMBER_OF_ROWS + 0) * NUMBER_OF_ACTS + A_UTSU};
        end = filter(moves, moves + 1, sizeof(uint16_t), remove_improper_promotion, NULL, moves);
        printf("remove_improper_promotion %s 11GIN_UTSU.\n", moves == end ? "REMOVES" : "preserves");

        end = filter(moves, moves + 1, sizeof(uint16_t), remove_forbidden_move, NULL, moves);
        printf("remove_forbidden_move %s 11GIN_UTSU.\n", moves == end ? "REMOVES" : "preserves");

        end = filter(moves, moves + 1, sizeof(uint16_t), remove_enter_outside, NULL, moves);
        printf("remove_enter_outside %s 11GIN_UTSU.\n", moves == end ? "REMOVES" : "preserves");

        end = filter(moves, moves + 1, sizeof(uint16_t), remove_enables_no_more_move, NULL, moves);
        printf("remove_enables_no_more_move %s 11GIN_UTSU.\n", moves == end ? "REMOVES" : "preserves");
    }
*/
    return 0;
}
#endif
