#ifndef __MATESEARCH_HPP__
#define __MATESEARCH_HPP__

#include "game.hpp"
#include "movelist.hpp"
#include "attack.hpp"
#include "movecheck.hpp"
#include "moveevasion.hpp"

#define DEBUG_MATE 0

namespace mate {

enum MateResult {
    MATED,
    NOT_MATED,
    UNKNOWN,
};
MateResult mate_and(game::Position &pos, int ply);
MateResult mate_or(game::Position &pos, int ply);

Move mate_search(game::Position &pos, int ply) {
    ASSERT(!attack::in_checked(pos));
    movelist::MoveList ml;
    gen::drop_check_moves(pos,ml);
    gen::pos_check_moves(pos,ml);
    const auto len = ml.len();
#if DEBUG_MATE
    Tee<<"mate_search\n";
    Tee<<pos<<std::endl;
#endif
    REP(i, len) {
#if DEBUG_MATE
        Tee<<move_str(ml[i])<<std::endl;
#endif
        auto next = pos.next(ml[i]);
        const auto result = mate_and(next, ply-1);
        if (result == MATED) {
            return ml[i];
        }
    }
    return MOVE_NONE;
}

Move mated_search(game::Position &pos, int ply) {
    ASSERT(attack::in_checked(pos));
    movelist::MoveList ml;
    gen::evasion_moves(pos,ml);
    const auto len = ml.len();
    REP(i, len) {
        auto next = pos.next(ml[i]);
        const auto result = mate_or(next, ply-1);
        if (result == NOT_MATED) {
            return ml[i];
        }
    }
    return MOVE_NONE;
}

MateResult mate_and(game::Position &pos, int ply) {
    ASSERT2(pos.is_ok(),{
        Tee<<"mate_and error\n";
        Tee<<pos<<std::endl;
    });
    if (ply < 0) {
#if DEBUG_MATE
        Tee<<"ply < 0 and"<<std::endl;
#endif
        return NOT_MATED;
    }
    if (pos.is_draw()) {
#if DEBUG_MATE
        Tee<<"is_draw and"<<std::endl;
#endif
        return NOT_MATED;
    }
    if (pos.is_lose()) {
#if DEBUG_MATE
        Tee<<"is_lose and"<<std::endl;
#endif
        return MATED;
    }
    if (!attack::in_checked(pos)) {
        Tee<<pos<<std::endl;
        ASSERT(false);
    }
    movelist::MoveList ml;
    gen::evasion_moves(pos, ml);
    const auto len = ml.len();
#if DEBUG_MATE
    Tee<<"mate_and\n";
    Tee<<pos<<std::endl;
#endif
    REP(i, len) {
#if DEBUG_MATE
        Tee<<move_str(ml[i])<<std::endl;
#endif
        auto next = pos.next(ml[i]);
        const auto result = mate_or(next, ply-1);
        if (result == NOT_MATED) {
            return NOT_MATED;
        }
    }
    return MATED;
}

MateResult mate_or(game::Position &pos, int ply) {
    ASSERT2(pos.is_ok(),{
        Tee<<"mate_or error\n";
        Tee<<pos<<std::endl;
    });
    if (ply < 0) {
#if DEBUG_MATE
        Tee<<"ply < 0"<<std::endl;
#endif
        return NOT_MATED;
    }
    if (pos.is_draw()) {
#if DEBUG_MATE
        Tee<<"draw or"<<std::endl;
#endif
        return NOT_MATED;
    }
    if (pos.is_lose()) {
#if DEBUG_MATE
        Tee<<"lose or"<<std::endl;
#endif
        return NOT_MATED;
    }
    if (attack::in_checked(pos)) {
#if DEBUG_MATE
        Tee<<"in_checked"<<std::endl;
#endif
        return NOT_MATED;
    }
    movelist::MoveList ml;
    gen::drop_check_moves(pos,ml);
    gen::pos_check_moves(pos,ml);
    const auto len = ml.len();
#if DEBUG_MATE
    Tee<<"mate_or\n";
    Tee<<pos<<std::endl;
#endif
    REP(i, len) {
#if DEBUG_MATE
        Tee<<move_str(ml[i])<<std::endl;
#endif
        auto next = pos.next(ml[i]);
        const auto result = mate_and(next, ply-1);
        if (result == MATED) {
            return MATED;
        }
    }
    return NOT_MATED;
}
bool test_mate2(std::string sfen, int ply, bool result) {
    auto pos = sfen::sfen(sfen);
    auto mv = mate_search(pos, ply);
    if (result) {
        return mv != MOVE_NONE;
    } else {
        return mv == MOVE_NONE;
    }
}
void test_mate() {
    ASSERT(test_mate2("1+RKss/p4/5/bPgG1/3rk b b - 81",5,false));
    ASSERT(test_mate2("4k/2g1p/Pb2R/1S3/KG3 b SRb - 11",5,false));
}

}
#endif