#ifndef __MOVEDROP_HPP__
#define __MOVEDROP_HPP__

#include "game.hpp"
#include "movelist.hpp"
#include "attack.hpp"

namespace gen {
    
template<bool is_exists = false> bool drop_moves(game::Position &pos, movelist::MoveList &ml) {
    const auto turn = pos.turn();
    const auto hand = pos.hand(turn);
    if (hand == HAND_NONE) {
        return false;
    }
    Piece hand_list[4] = {EMPTY};
    int sp = 0;
    if (has_piece(hand, ROOK)) {
        hand_list[sp++] = ROOK;
    }
    if (has_piece(hand, BISHOP)) {
        hand_list[sp++] = BISHOP;
    }
    if (has_piece(hand, GOLD)) {
        hand_list[sp++] = GOLD;
    }
    if (has_piece(hand, SILVER)) {
        hand_list[sp++] = SILVER;
    }
#define ADD_PAWN(r) do {\
    const auto to = square(file, r);\
    if (pos.square(to) == COLOR_EMPTY && !is_mate_with_pawn_drop(to, pos)) {\
        if (is_exists) { return true; }\
        ml.add(move(to, PAWN)); \
    }\
} while(0)
    // 歩の打つ手生成
    if (has_piece(hand, PAWN)) {
        REP_FILE(file){
            if (pos.exists_pawn(turn,file)) { continue; }
            if (turn == BLACK) {
                ADD_PAWN(RANK_2);
                ADD_PAWN(RANK_3);
                ADD_PAWN(RANK_4);
                ADD_PAWN(RANK_5);
            } else {
                ADD_PAWN(RANK_4);
                ADD_PAWN(RANK_3);
                ADD_PAWN(RANK_2);
                ADD_PAWN(RANK_1);
            }
        }
    }
#undef ADD_PAWN
    // その他の持ち駒
    for(auto *p = SQUARE_INDEX; *p != SQ_WALL; ++p) {
        const auto to = *p;
        const auto color_piece = pos.square(to);
        if (color_piece != COLOR_EMPTY) { continue; }
        if (is_exists) {
            return true;
        }
        switch(sp) {
            case 4:
                ml.add(move(to, hand_list[3]));
                /* Falls through. */
            case 3:
                ml.add(move(to, hand_list[2]));
                /* Falls through. */
            case 2:
                ml.add(move(to, hand_list[1]));
                /* Falls through. */
            case 1:
                ml.add(move(to, hand_list[0]));
                /* Falls through. */
            default:
                break;
        }
    }
    return false;
}

}
#endif