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

int main() {
    const unsigned int N = NUMBER_OF_COLUMNS * NUMBER_OF_ROWS * NUMBER_OF_KOMA * NUMBER_OF_MOVES;
    uint16_t *const all_moves = malloc(sizeof(uint16_t) * N);
    int i;
    for (i = 0; i < N; ++i) {
        struct move_detail move = move_detail(i);

        printf("[%5d] %d %d %s %s\n", i, move.suji + 1, move.dan + 1, _str_koma(move.koma), _str_move(move.move));
        if (i == 4096) break;

        all_moves[i] = i;
    }
    free(all_moves);
    return 0;
}
#endif
