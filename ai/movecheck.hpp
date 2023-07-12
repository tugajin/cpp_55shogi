#ifndef __MOVECHECK_HPP__
#define __MOVECHECK_HPP__

#include "game.hpp"
#include "movelist.hpp"
#include "attack.hpp"

namespace gen {

void pos_check_moves(game::Position &pos, movelist::MoveList &ml) {
    const auto turn = pos.turn();
    const auto opp = change_turn(turn);
    const auto opp_king_sq = pos.king_sq(opp);
    const auto move_flag = (turn == BLACK) ? (BLACK_FLAG | COLOR_WALL_FLAG) : (WHITE_FLAG | COLOR_WALL_FLAG);
    {
        const auto size = pos.piece_list_size(turn, PAWN);
        REP(index, size) {
            const auto from = pos.piece_list(turn, PAWN, index);
            const auto to = (turn == BLACK) ? from + INC_UP : from + INC_DOWN;
            if (attack::can_move(pos.square(to), move_flag)) {
                if (attack::is_discover(from,to,turn,pos)) { continue; }
                if (sq_is_prom(to, turn)) {
                    const auto cp = prom(pos.square(from));
                    if (attack::pseudo_attack(cp, opp_king_sq - to) || attack::is_discover(from, to, opp, pos)) { 
                        ml.add(move(from, to, true));
                    }
                } else {
                    const auto cp = pos.square(from);
                    if (attack::pseudo_attack(cp, opp_king_sq - to) || attack::is_discover(from, to, opp, pos)) { 
                        ml.add(move(from, to));
                    }
                }
            }
        }
    }

#define ADD_MOVE_PROM(dir) do {\
            const auto to = from + (dir);\
            if (attack::can_move(pos.square(to), move_flag)) {\
                if (attack::is_discover(from,to,turn,pos)) { continue; }\
                if (sq_is_prom(to, turn) || sq_is_prom(from, turn)) {\
                    const auto cp = pos.square(from);\
                    if (attack::is_discover(from, to, opp, pos)){\
                        ml.add(move(from, to, true));\
                        ml.add(move(from, to));\
                        continue;\
                    }\
                    if (attack::pseudo_attack(cp, opp_king_sq - to)) { \
                        ml.add(move(from, to));\
                    } \
                    if (attack::pseudo_attack(prom(cp), opp_king_sq - to)) {\
                        ml.add(move(from, to, true));\
                    }\
                } else {\
                    const auto cp = pos.square(from);\
                    if (attack::pseudo_attack(cp, opp_king_sq - to) || attack::is_discover(from, to, opp, pos)) { \
                        ml.add(move(from, to));\
                    }\
                }\
            }\
}while(false)
#define ADD_MOVE(dir) do {\
            const auto to = from + (dir);\
            if (attack::can_move(pos.square(to), move_flag)) {\
                if (attack::is_discover(from,to,turn,pos)) { continue; }\
                const auto cp = pos.square(from);\
                if (attack::pseudo_attack(cp, opp_king_sq - to) || attack::is_discover(from, to, opp, pos)) { \
                    ml.add(move(from, to));\
                } \
            }\
}while(false)
#define ADD_MOVE2(dir) do {\
            const auto to = from + (dir);\
            if (attack::can_move(pos.square(to), move_flag)) {\
                if (attack::is_discover(from,to,turn,pos)) { continue; }\
                const auto cp = pos.square(from);\
                if (attack::slider_attack(to, opp_king_sq, cp, pos) || attack::is_discover(from, to, opp, pos)) { \
                    ml.add(move(from, to));\
                } \
            }\
}while(false)
    {
        const auto size = pos.piece_list_size(turn, SILVER);
        REP(index, size) {
            const auto from = pos.piece_list(turn, SILVER, index);
            if (turn == BLACK) {
                ADD_MOVE_PROM(INC_UP);
            } else {
                ADD_MOVE_PROM(INC_DOWN);
            }
            ADD_MOVE_PROM(INC_LEFTUP);
            ADD_MOVE_PROM(INC_RIGHTUP);
            ADD_MOVE_PROM(INC_LEFTDOWN);
            ADD_MOVE_PROM(INC_RIGHTDOWN);
        }
    }

    {
        const auto size = pos.piece_list_size(turn, GOLD);
        REP(index, size) {
            const auto from = pos.piece_list(turn, GOLD, index);
            ADD_MOVE(INC_UP);
            ADD_MOVE(INC_DOWN);
            ADD_MOVE(INC_LEFT);
            ADD_MOVE(INC_RIGHT);
            if (turn == BLACK) {
                ADD_MOVE(INC_LEFTUP);
                ADD_MOVE(INC_RIGHTUP);
            } else {
                ADD_MOVE(INC_LEFTDOWN);
                ADD_MOVE(INC_RIGHTDOWN);
            }
        }
    }

    {
        const auto size = pos.piece_list_size(turn, PPAWN);
        REP(index, size) {
            const auto from = pos.piece_list(turn, PPAWN, index);
            ADD_MOVE(INC_UP);
            ADD_MOVE(INC_DOWN);
            ADD_MOVE(INC_LEFT);
            ADD_MOVE(INC_RIGHT);
            if (turn == BLACK) {
                ADD_MOVE(INC_LEFTUP);
                ADD_MOVE(INC_RIGHTUP);
            } else {
                ADD_MOVE(INC_LEFTDOWN);
                ADD_MOVE(INC_RIGHTDOWN);
            }
        }
    }

    {
        const auto size = pos.piece_list_size(turn, PSILVER);
        REP(index, size) {
            const auto from = pos.piece_list(turn, PSILVER, index);
            ADD_MOVE(INC_UP);
            ADD_MOVE(INC_DOWN);
            ADD_MOVE(INC_LEFT);
            ADD_MOVE(INC_RIGHT);
            if (turn == BLACK) {
                ADD_MOVE(INC_LEFTUP);
                ADD_MOVE(INC_RIGHTUP);
            } else {
                ADD_MOVE(INC_LEFTDOWN);
                ADD_MOVE(INC_RIGHTDOWN);
            }
        }
    }
#undef ADD_MOVE_PROM
    {
#define ADD_MOVE_KING(dir) do {\
            const auto to = from + (dir);\
            if (attack::can_move(pos.square(to), move_flag)) {\
                if (attack::is_attacked(pos,to,opp)) { continue; }\
                if (attack::is_discover(from, to, opp, pos)) {\
                    ml.add(move(from, to));\
                }\
            }\
}while(false)
        const auto from = pos.king_sq(turn);
        ADD_MOVE_KING(INC_UP);
        ADD_MOVE_KING(INC_LEFTUP);
        ADD_MOVE_KING(INC_LEFT);
        ADD_MOVE_KING(INC_LEFTDOWN);
        ADD_MOVE_KING(INC_DOWN);
        ADD_MOVE_KING(INC_RIGHTDOWN);
        ADD_MOVE_KING(INC_RIGHT);
        ADD_MOVE_KING(INC_RIGHTUP);
    }
#undef ADD_MOVE_KING

#define ADD_MOVE_SLIER_PROM(dir) do {\
            /*Tee<<"start:"<<sq_str(from)<<":"<<dir<<std::endl;*/\
            const auto inc = dir_to_inc(dir);\
            /*Tee<<"inc:"<<inc<<std::endl;*/\
            const auto neighbor_sq = pos.neighbor(from, dir);\
            /*Tee<<"nei:"<<neighbor_sq<<std::endl;*/\
            auto to = from + inc;\
            /*Tee<<"to:"<<sq_str(to)<<std::endl;*/\
            for (; to != neighbor_sq; to += inc) {\
                ASSERT2(to >= 0 && to < SQ_END, { Tee<<"error add_move_slider\n"; });\
                if (attack::is_discover(from,to,turn,pos)) { continue; }\
                /*Tee<<"passed discover"<<std::endl;*/\
                auto cp = pos.square(from);\
                if ((sq_is_prom(to, turn) || sq_is_prom(from, turn))) {\
                    cp = prom(cp);\
                    if (attack::slider_attack(to, opp_king_sq, cp, pos) || attack::is_discover(from,to,opp,pos)) {\
                        /*Tee<<"add"<<std::endl;*/\
                        ml.add(move(from,to, true));\
                    }\
                } else {\
                    if (attack::slider_attack(to, opp_king_sq, cp, pos) || attack::is_discover(from,to,opp,pos)) {\
                        /*Tee<<"add2"<<std::endl;*/\
                        ml.add(move(from,to));\
                    }\
                }\
            }\
            /*Tee<<"loop end"<<to<<std::endl;*/\
            if (attack::can_move(pos.square(to),move_flag)) {\
                /*Tee<<"passed can_move"<<to<<std::endl;*/\
                ASSERT2(to >= 0 && to < SQ_END, { Tee<<"error add_move_slider2\n"; });\
                if (attack::is_discover(from,to,turn,pos)) { continue; }\
                /*Tee<<"passed discover2"<<to<<std::endl;*/\
                auto cp = pos.square(from);\
                if ((sq_is_prom(to, turn) || sq_is_prom(from, turn))) {\
                    cp = prom(cp);\
                    if (attack::slider_attack(to, opp_king_sq, cp, pos) || attack::is_discover(from,to,opp,pos)) {\
                        /*Tee<<"add3"<<std::endl;*/\
                        ml.add(move(from,to, true));\
                    }\
                } else {\
                    if (attack::slider_attack(to, opp_king_sq, cp, pos) || attack::is_discover(from,to,opp,pos)) {\
                        /*Tee<<"add4"<<std::endl;*/\
                        ml.add(move(from,to));\
                    }\
                }\
            }\
}while(false)
#define ADD_MOVE_SLIER(dir) do {\
            const auto inc = dir_to_inc(dir);\
            const auto neighbor_sq = pos.neighbor(from, dir);\
            auto to = from + inc;\
            for (; to != neighbor_sq; to += inc) {\
                if (attack::is_discover(from,to,turn,pos)) { continue; }\
                const auto cp = pos.square(from);\
                ASSERT2(to >= 0 && to < SQ_END, { Tee<<"error add_move_slider\n"; });\
                if (attack::slider_attack(to, opp_king_sq, cp, pos) || attack::is_discover(from,to,opp,pos)) {\
                    ml.add(move(from,to));\
                }\
            }\
            if (attack::can_move(pos.square(to),move_flag)) {\
                if (attack::is_discover(from,to,turn,pos)) { continue; }\
                const auto cp = pos.square(from);\
                ASSERT2(to >= 0 && to < SQ_END, { Tee<<"error add_move_slider2\n"; });\
                if (attack::slider_attack(to, opp_king_sq, cp, pos) || attack::is_discover(from,to,opp,pos)) {\
                    ml.add(move(from,to));\
                }\
            }\
}while(false)

    {
        const auto size = pos.piece_list_size(turn, BISHOP);
        REP(index, size) {
            const auto from = pos.piece_list(turn, BISHOP, index);
            ADD_MOVE_SLIER_PROM(1);
            ADD_MOVE_SLIER_PROM(3);
            ADD_MOVE_SLIER_PROM(5);
            ADD_MOVE_SLIER_PROM(7);
        }
    }

    {
        const auto size = pos.piece_list_size(turn, ROOK);
        REP(index, size) {
            const auto from = pos.piece_list(turn, ROOK, index);
            ADD_MOVE_SLIER_PROM(0);
            ADD_MOVE_SLIER_PROM(2);
            ADD_MOVE_SLIER_PROM(4);
            ADD_MOVE_SLIER_PROM(6);
        }
    }

    {
        const auto size = pos.piece_list_size(turn, PBISHOP);
        REP(index, size) {
            const auto from = pos.piece_list(turn, PBISHOP, index);
            ADD_MOVE_SLIER(1);
            ADD_MOVE_SLIER(3);
            ADD_MOVE_SLIER(5);
            ADD_MOVE_SLIER(7);
            ADD_MOVE2(INC_UP);
            ADD_MOVE2(INC_DOWN);
            ADD_MOVE2(INC_LEFT);
            ADD_MOVE2(INC_RIGHT);
        }
    }

    {
        const auto size = pos.piece_list_size(turn, PROOK);
        REP(index, size) {
            const auto from = pos.piece_list(turn, PROOK, index);
            ADD_MOVE_SLIER(0);
            ADD_MOVE_SLIER(2);
            ADD_MOVE_SLIER(4);
            ADD_MOVE_SLIER(6);
            ADD_MOVE2(INC_LEFTUP);
            ADD_MOVE2(INC_LEFTDOWN);
            ADD_MOVE2(INC_RIGHTUP);
            ADD_MOVE2(INC_RIGHTDOWN);
        }
    }

#undef ADD_MOVE_SLIER_PROM
#undef ADD_MOVE_SLIER
#undef ADD_MOVE
#undef ADD_MOVE2
    
}

void drop_check_moves(game::Position &pos, movelist::MoveList &ml) {
    const auto me = pos.turn();
    const auto hand = pos.hand(me);
    
    if (hand == HAND_NONE) { return; }
    
    const auto opp = change_turn(me);
    const auto opp_king_sq = pos.king_sq(opp);
#define ADD_MOVE(p) do {\
    ASSERT(p != PAWN);\
    if (has_piece(hand,p)) {\
        ml.add(move(to,p));\
    }\
}while(0)
#define ADD_PAWN do {\
    if (has_piece(hand,PAWN) && !pos.exists_pawn(me,sq_file(to)) && !gen::is_mate_with_pawn_drop(to, pos)) {\
        ml.add(move(to,PAWN));\
    }\
}while(0)
    {
        const auto inc = INC_UP;
        auto to = opp_king_sq + inc;
        if (pos.square(to) == COLOR_EMPTY) {
            ADD_MOVE(GOLD);
            ADD_MOVE(ROOK);
            if (me == WHITE) {
                ADD_MOVE(SILVER);
                ADD_PAWN;
            }
            for (to += inc; pos.square(to) == COLOR_EMPTY; to+=inc) {
                ADD_MOVE(ROOK);
            }
        }
    }
    {
        const auto inc = INC_LEFTUP;
        auto to = opp_king_sq + inc;
        if (pos.square(to) == COLOR_EMPTY) {
            ADD_MOVE(SILVER);
            ADD_MOVE(BISHOP);
            if (me == WHITE) {
                ADD_MOVE(GOLD);
            }
            for (to += inc; pos.square(to) == COLOR_EMPTY; to+=inc) {
                ADD_MOVE(BISHOP);
            }
        } 
    }
    {
        const auto inc = INC_LEFT;
        auto to = opp_king_sq + inc;
        if (pos.square(to) == COLOR_EMPTY) {
            ADD_MOVE(GOLD);
            ADD_MOVE(ROOK);
            for (to += inc; pos.square(to) == COLOR_EMPTY; to+=inc) {
                ADD_MOVE(ROOK);
            }
        }
    }
    {
        const auto inc = INC_LEFTDOWN;
        auto to = opp_king_sq + inc;
        if (pos.square(to) == COLOR_EMPTY) {
            ADD_MOVE(SILVER);
            ADD_MOVE(BISHOP);
            if (me == BLACK) {
                ADD_MOVE(GOLD);
            }
            for (to += inc; pos.square(to) == COLOR_EMPTY; to+=inc) {
                ADD_MOVE(BISHOP);
            }
        } 
    }
    {
        const auto inc = INC_DOWN;
        auto to = opp_king_sq + inc;
        if (pos.square(to) == COLOR_EMPTY) {
            ADD_MOVE(GOLD);
            ADD_MOVE(ROOK);
            if (me == BLACK) {
                ADD_MOVE(SILVER);
                ADD_PAWN;
            }
            for (to += inc; pos.square(to) == COLOR_EMPTY; to+=inc) {
                ADD_MOVE(ROOK);
            }
        }
    }
    {
        const auto inc = INC_RIGHTDOWN;
        auto to = opp_king_sq + inc;
        if (pos.square(to) == COLOR_EMPTY) {
            ADD_MOVE(SILVER);
            ADD_MOVE(BISHOP);
            if (me == BLACK) {
                ADD_MOVE(GOLD);
            }
            for (to += inc; pos.square(to) == COLOR_EMPTY; to+=inc) {
                ADD_MOVE(BISHOP);
            }
        }
    }
    {
        const auto inc = INC_RIGHT;
        auto to = opp_king_sq + inc;
        if (pos.square(to) == COLOR_EMPTY) {
            ADD_MOVE(GOLD);
            ADD_MOVE(ROOK);
            for (to += inc; pos.square(to) == COLOR_EMPTY; to+=inc) {
                ADD_MOVE(ROOK);
            }
        } 
    }
    {
        const auto inc = INC_RIGHTUP;
        auto to = opp_king_sq + inc;
        if (pos.square(to) == COLOR_EMPTY) {
            ADD_MOVE(SILVER);
            ADD_MOVE(BISHOP);
            if (me == WHITE) {
                ADD_MOVE(GOLD);
            }
            for (to += inc; pos.square(to) == COLOR_EMPTY; to+=inc) {
                ADD_MOVE(BISHOP);
            }
        }
    }
#undef ADD_MOVE
#undef ADD_PAWN
}

}
#endif