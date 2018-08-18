#include "move.h"

#include <stdint.h>
#include <stdlib.h>

#define NUMBER_OF_COLUMNS 9
#define NUMBER_OF_ROWS 9

static const uint16_t _allowed_moves[] = {
#include "allowed-moves"
};

static struct move describe(unsigned move) {
    struct move result;
    div_t d = {move};
    d = div(d.quot, NUMBER_OF_ACTS);
    result.act = d.rem;
    d = div(d.quot, NUMBER_OF_ROWS);
    result.dan = d.rem;
    d = div(d.quot, NUMBER_OF_COLUMNS);
    result.suji = d.rem;
    d = div(d.quot, NUMBER_OF_KOMA);
    result.koma = d.rem;
    return result;
}

struct move move_describe(unsigned code) {
    return describe(_allowed_moves[code]);
}

#ifdef UNITTEST_
#include <stdio.h>
#include <memory.h>

#include "filter.h"

static const char* _str_koma(unsigned koma) {
    switch (koma) {
    case K_FU: return "歩";
    case K_TO: return "と";
    case K_KYOU: return "香";
    case K_NARIKYOU: return "杏";
    case K_KEI: return "桂";
    case K_NARIKEI: return "圭";
    case K_GIN: return "銀";
    case K_NARIGIN: return "全";
    case K_KIN: return "金";
    case K_KAKU: return "角";
    case K_UMA: return "馬";
    case K_HISHA: return "飛";
    case K_RYU: return "竜";
    case K_GYOKU: return "玉";
    default: fprintf(stderr, "Invalid koma: %d\n", koma); return "";
    }
}

static const char* _str_move(unsigned move) {
    switch (move) {
    case A_AGARU: return "";
    case A_NARU: return "成";
    case A_MIGI: return "右";
    case A_MIGI_NARU: return "右成";
    case A_HIDARI: return "左";
    case A_HIDARI_NARU: return "左成";
    case A_MIGI_YORU: return "寄右";
    case A_MIGI_YORI_NARU: return "寄右成";
    case A_HIDARI_YORU: return "寄左";
    case A_HIDARI_YORI_NARU: return "寄左成";
    case A_MIGI_HIKU: return "引右";
    case A_MIGI_HIKI_NARU: return "引右成";
    case A_HIDARI_HIKU: return "引左";
    case A_HIDARI_HIKI_NARU: return "引左成";
    case A_HIKU: return "引";
    case A_HIKI_NARU: return "引成";
    case A_UTSU: return "打";
    default: fprintf(stderr, "Invalid act: %d\n", move); return "";
    }
}

static unsigned remove_improper_promotion(const void *elem, void *context) {
    const struct move move = describe(*(uint16_t *)elem);
    switch (move.koma) {
    case K_FU:
    case K_KYOU:
        switch (move.act) {
        case A_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return 0; /* OK */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_MIGI_NARU: case A_HIDARI_NARU: case A_MIGI_YORI_NARU: case A_HIDARI_YORI_NARU:
        case A_MIGI_HIKI_NARU: case A_HIDARI_HIKI_NARU: case A_HIKI_NARU:
            return !0; /* Invalid move. */
        default:
            return 0; /* DONT CARE */
        }
    case K_KEI:
        switch (move.act) {
        case A_MIGI_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return move.suji == SUJI_1; /* OK; SUJI 1 is right most, so Keima can't be moved in. */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_HIDARI_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return move.suji == SUJI_9; /* OK; SUJI 9 is left most, so Keima can't be moved in. */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_NARU: case A_MIGI_YORI_NARU: case A_HIDARI_YORI_NARU:
        case A_MIGI_HIKI_NARU: case A_HIDARI_HIKI_NARU: case A_HIKI_NARU:
            return !0; /* Invalid move. */
        default:
            return 0; /* DONT CARE */
        }
    case K_GIN:
        switch (move.act) {
        case A_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return 0; /* OK */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_MIGI_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return move.suji == SUJI_1; /* OK; SUJI 1 is right most, so Keima can't be moved in. */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_HIDARI_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return move.suji == SUJI_9; /* OK; SUJI 9 is left most, so Keima can't be moved in. */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_MIGI_HIKI_NARU:
            switch (move.dan) {
            case DAN_8: case DAN_7:
                return move.suji == SUJI_1;
            default:
                return !0; /* Can not be backed in DAN 9. DAN between 6 to 1 are out of promotion area. */
            }
        case A_HIDARI_HIKI_NARU:
            switch (move.dan) {
            case DAN_8: case DAN_7:
                return move.suji == SUJI_9;
            default:
                return !0;
            }
        case A_MIGI_YORI_NARU: case A_HIDARI_YORI_NARU: case A_HIKI_NARU:
            return !0; /* Invalid move. */
        default:
            return 0; /* DONT CARE */
        }
    case K_KAKU:
        switch (move.act) {
        case A_MIGI_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return move.suji == SUJI_1; /* OK; SUJI 1 is right most, so Keima can't be moved in. */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_HIDARI_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return move.suji == SUJI_9; /* OK; SUJI 9 is left most, so Keima can't be moved in. */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_MIGI_HIKI_NARU:
            switch (move.dan) {
            case DAN_8: case DAN_7:
                return move.suji == SUJI_1;
            default:
                return !0; /* Can not be backed in DAN 9. DAN between 6 to 1 are out of promotion area. */
            }
        case A_HIDARI_HIKI_NARU:
            switch (move.dan) {
            case DAN_8: case DAN_7:
                return move.suji == SUJI_9;
            default:
                return !0;
            }
        case A_NARU: case A_MIGI_YORI_NARU: case A_HIDARI_YORI_NARU: case A_HIKI_NARU:
            return !0; /* Invalid move. */
        default:
            return 0; /* DONT CARE */
        }
    case K_HISHA:
        switch (move.act) {
        case A_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return 0; /* OK */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_MIGI_YORI_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return move.suji == SUJI_1; /* OK */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_HIDARI_YORI_NARU:
            switch (move.dan) {
            case DAN_9: case DAN_8: case DAN_7:
                return move.suji == SUJI_9; /* OK */
            default:
                return !0; /* Out of promotion area. */
            }
        case A_HIKI_NARU:
            switch (move.dan) {
            case DAN_8: case DAN_7:
                return 0; /* OK */
            default:
                return !0;
            }
        case A_MIGI_NARU: case A_HIDARI_NARU: case A_MIGI_HIKI_NARU: case A_HIDARI_HIKI_NARU:
            return !0; /* NG */
        default:
            return 0; /* DONT CARE */
        }
    default:
        switch (move.act) {
        case A_NARU: case A_MIGI_NARU: case A_HIDARI_NARU: case A_MIGI_YORI_NARU: case A_HIDARI_YORI_NARU:
        case A_MIGI_HIKI_NARU: case A_HIDARI_HIKI_NARU: case A_HIKI_NARU:
            return !0; /* Forbidden promotion. */
        default:
            return 0; /* DONT CARE */
        }
    }
}

static unsigned remove_forbidden_move(const void *elem, void *context) {
    const struct move move = describe(*(uint16_t *)elem);
    switch (move.koma) {
    case K_FU:
    case K_KYOU:
        switch (move.act) {
        case A_AGARU:
        case A_NARU:
        case A_UTSU:
            return 0;
        default:
            return !0;
        }
    case K_KEI:
        switch (move.act) {
        case A_MIGI: case A_HIDARI:
        case A_MIGI_NARU: case A_HIDARI_NARU:
        case A_UTSU:
            return 0;
        default:
            return !0;
        }
    case K_GIN:
        switch (move.act) {
        case A_AGARU: case A_MIGI: case A_HIDARI: case A_MIGI_HIKU: case A_HIDARI_HIKU:
        case A_NARU: case A_MIGI_NARU: case A_HIDARI_NARU: case A_MIGI_HIKI_NARU: case A_HIDARI_HIKI_NARU:
        case A_UTSU:
            return 0;
        default:
            return !0;
        }
    case K_KIN:
        switch (move.act) {
        case A_AGARU: case A_MIGI: case A_HIDARI: case A_MIGI_YORU: case A_HIDARI_YORU: case A_HIKU:
        case A_UTSU:
            return 0;
        default:
            return !0;
        }
    case K_TO:
    case K_NARIKYOU:
    case K_NARIKEI:
    case K_NARIGIN:
        switch (move.act) {
        case A_AGARU: case A_MIGI: case A_HIDARI: case A_MIGI_YORU: case A_HIDARI_YORU: case A_HIKU:
            return 0;
        default:
            return !0;
        }
    case K_KAKU:
        switch (move.act) {
        case A_MIGI: case A_HIDARI: case A_MIGI_HIKU: case A_HIDARI_HIKU:
        case A_UTSU:
            return 0;
        default:
            return !0;
        }
    case K_HISHA:
        switch (move.act) {
        case A_AGARU: case A_MIGI_YORU: case A_HIDARI_YORU: case A_HIKU:
        case A_UTSU:
            return 0;
        default:
            return !0;
        }
    case K_UMA:
    case K_RYU:
    case K_GYOKU:
        return move.act == A_UTSU;
    }
}

static unsigned remove_enter_outside(const void *elem, void *context) {
    const struct move move = describe(*(uint16_t *)elem);
    unsigned result = 0;
    switch (move.dan) {
    case DAN_9:
        switch (move.act) {
        case A_MIGI_HIKU: case A_HIDARI_HIKU: case A_HIKU:
            result = !0;
            /* $fall through$ */
        default:
            break;
        }
        break;
    case DAN_2:
        switch (move.act) {
        case A_MIGI: case A_HIDARI:
            result = move.koma == K_KEI;
            /* $fall through$ */
        default:
            break;
        }
        break;
    case DAN_1:
        switch (move.act) {
        case A_AGARU: case A_MIGI: case A_HIDARI:
            result = !0;
            /* $fall through$ */
        default:
            break;
        }
        /* $fall through$ */
    default:
        break;
    }
    switch (move.suji) {
    case SUJI_9:
        switch (move.act) {
        case A_HIDARI: case A_HIDARI_YORU: case A_HIDARI_HIKU:
            result |= !0;
            /* $fall through$ */
        default:
            break;
        }
        break;
    case SUJI_1:
        switch (move.act) {
        case A_MIGI: case A_MIGI_YORU: case A_MIGI_HIKU:
            result |= !0;
            /* $fall through$ */
        default:
            break;
        }
        /* $fall through$ */
    default:
        break;
    }
    return result;
}

static unsigned remove_enables_no_more_move(const void *elem, void *context) {
    const struct move move = describe(*(uint16_t *)elem);
    switch (move.koma) {
    case K_FU:
    case K_KYOU:
        return move.dan == DAN_9 && (move.act == A_AGARU || move.act == A_UTSU);
    case K_KEI:
        return (move.dan == DAN_9 || move.dan == DAN_8) && (move.act == A_MIGI || move.act == A_HIDARI || move.act == A_UTSU);
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
        struct move move = describe(*it);
        printf("%d%d%s%s\n", 9 - move.suji, 9 - move.dan, _str_koma(move.koma), _str_move(move.act));
    }

    /* # of allowed_moves equals to NUMBER_OF_MOVES */
    if (sizeof(_allowed_moves)/sizeof(_allowed_moves[0]) != NUMBER_OF_MOVES) {
        return 1;
    }

    /* # of all_moves (filtered) equals to NUMBER_OF_MOVES */
    if (end - all_moves != NUMBER_OF_MOVES) {
        fprintf(stderr, "Number of moves are mismatch. (%d v.s. %ld)\n", NUMBER_OF_MOVES, end - all_moves);
        return 1;
    }

    /* all_moves (filtered) and _allowed_moves has just same elements. */
    if (memcmp(all_moves, _allowed_moves, sizeof(_allowed_moves))) {
        fprintf(stderr, "Elements in moves are mismatch.\n");
        return 1;
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
