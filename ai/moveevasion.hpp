#ifndef __MOVEEVASION_HPP__
#define __MOVEEVASION_HPP__

#include "game.hpp"
#include "movelist.hpp"
#include "attack.hpp"

namespace gen {

template<bool is_exists = false, bool add_drops> bool add_moves_to(game::Position &pos, movelist::MoveList &ml, const Square to) {
    const auto me = pos.turn();
    const auto king_sq = pos.king_sq(me);
#define ADD_MOVE(f) do {\
            const auto from = (f);\
            if (from == king_sq) { continue; }\
            const auto cp = pos.square(from);\
            /*Tee<<"from:"<<sq_str(from)<<std::endl;*/\
            /*Tee<<"cp:"<<cp<<std::endl;*/\
            if (color_is_eq(me, pos.square(from))) {\
                const auto pc = to_piece(cp);\
                /*Tee<<"pc:"<<piece_str(pc)<<std::endl;*/\
                switch(pc) {\
                    case PAWN: {\
                        if (pseudo_attack(cp,to-from) && !attack::is_pinned(from,me,pos)) {\
                            if (is_exists) { return true; }\
                            ml.add(move(from,to,(sq_is_prom(to, me))));\
                        }\
                        break;\
                    }\
                    case SILVER:{\
                        if (pseudo_attack(cp,to-from) && !attack::is_pinned(from,me,pos)) {\
                            if (is_exists) { return true; }\
                            if ((sq_is_prom(to, me) || sq_is_prom(from, me))) { ml.add(move(from,to, true)); }\
                            ml.add(move(from,to));\
                        }\
                        break;\
                    }\
                    case GOLD:\
                    case PPAWN:\
                    case PSILVER:{\
                        if (pseudo_attack(cp,to-from) && !attack::is_pinned(from,me,pos)) {\
                            if (is_exists) { return true; }\
                            ml.add(move(from,to));\
                        }\
                        break;\
                    }\
                    case BISHOP:\
                    case ROOK: {\
                        if (pseudo_attack(cp,to-from) && !attack::is_pinned(from,me,pos)) {\
                            if (is_exists) { return true; }\
                            ml.add(move(from,to,(sq_is_prom(to, me) || sq_is_prom(from, me))));\
                        }\
                        break;\
                    }\
                    case PBISHOP:\
                    case PROOK:{\
                        if (pseudo_attack(cp,to-from) && !attack::is_pinned(from,me,pos)) {\
                            if (is_exists) { return true; }\
                            ml.add(move(from,to));\
                        }\
                        break;\
                    }\
                    default: {\
                        ASSERT(false);\
                        assert(false);\
                        break;\
                    }\
                }\
            }\
}while(false)
    Square neighbor_sq[8] = {};
    if (add_drops) {
        pos.occupy_empty(to, neighbor_sq);
    } else {
        pos.occupy_fill(to, neighbor_sq);
    }
    ADD_MOVE(neighbor_sq[0]);
    ADD_MOVE(neighbor_sq[1]);
    ADD_MOVE(neighbor_sq[2]);
    ADD_MOVE(neighbor_sq[3]);
    ADD_MOVE(neighbor_sq[4]);
    ADD_MOVE(neighbor_sq[5]);
    ADD_MOVE(neighbor_sq[6]);
    ADD_MOVE(neighbor_sq[7]);
#undef ADD_MOVE
    if (!add_drops) {
        return false;
    }
    ASSERT2(pos.square(to) == COLOR_EMPTY,{
        Tee<<"pos not empty error\n";
        Tee<<pos<<std::endl;
        Tee<<sq_str(to)<<std::endl;
    });
    const auto hand = pos.hand(me);
    const auto rank_first = (me == BLACK) ? RANK_1 : RANK_5;
    if (has_piece(hand, PAWN) 
        && (sq_rank(to) != rank_first)
        && !pos.exists_pawn(me,sq_file(to))
        && !is_mate_with_pawn_drop(to,pos)
     ) {
        if (is_exists) { return true; }
        ml.add(move(to,PAWN));
    }
    if (has_piece(hand, SILVER)) {
        if (is_exists) { return true; }
        ml.add(move(to,SILVER));
    }
    if (has_piece(hand, GOLD)) {
        if (is_exists) { return true; }
        ml.add(move(to, GOLD));
    }
    if (has_piece(hand, BISHOP)) {
        if (is_exists) { return true; }
        ml.add(move(to, BISHOP));
    }
    if (has_piece(hand, ROOK)) {
        if (is_exists) { return true; }
        ml.add(move(to, ROOK));
    }
    return false;
}

template<bool is_exists = false> bool evasion_moves(game::Position &pos, movelist::MoveList &ml) {
    const auto me = pos.turn();
    const auto opp = change_turn(me);
    const auto king_sq = pos.king_sq(me);
    const auto move_flag = (me == BLACK) ? (BLACK_FLAG | COLOR_WALL_FLAG) : (WHITE_FLAG | COLOR_WALL_FLAG);
    attack::Checker checker(pos);
    ASSERT2(checker.num !=0,{
        Tee<<"evasion error\n";
        Tee<<pos<<std::endl;
    })
    {
        const auto from = pos.king_sq(me);
#define ADD_KING(in) do {\
    /*Tee<<"start\n";*/\
    /*Tee<<in<<std::endl;*/\
    if ((in) != -checker.inc[0] && (in) != -checker.inc[1]) {\
        const auto to = from + (in);\
        /*Tee<<"to:"<<sq_str(to)<<std::endl;*/\
        if (attack::can_move(pos.square(to),move_flag) && !attack::is_attacked(pos,to,opp)) {\
            if (is_exists) { return true; }\
            /*Tee<<"add:"<<sq_str(to)<<std::endl;*/\
            ml.add(move(from, to));\
        }\
    }\
}while(0)
        ADD_KING(INC_UP);
        ADD_KING(INC_LEFTUP);
        ADD_KING(INC_LEFT);
        ADD_KING(INC_LEFTDOWN);
        ADD_KING(INC_DOWN);
        ADD_KING(INC_RIGHTDOWN);
        ADD_KING(INC_RIGHT);
        ADD_KING(INC_RIGHTUP);
#undef ADD_KING
    }
    if (checker.num == 2) {
        return false;
    }
    ASSERT(checker.num == 1);
    ASSERT(sq_is_ok(checker.from[0]));
    //capture checker
    const auto result = add_moves_to<is_exists, false>(pos,ml,checker.from[0]);
    if (is_exists && result) { return true; }
    //intercept 
    const auto inc = checker.inc[0];
    if (inc != INC_NONE) {
        for (auto to = king_sq + inc; to != checker.from[0]; to += inc) {
            const auto result = add_moves_to<is_exists, true>(pos,ml,to);
            if (is_exists && result) { return true; }
        }
    }    
    return false;
}

}
#endif