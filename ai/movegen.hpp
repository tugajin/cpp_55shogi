#ifndef __MOVEGEN_HPP__
#define __MOVEGEN_HPP__

#include "game.hpp"
#include "movelist.hpp"
#include "attack.hpp"

namespace gen {

template<bool is_exists = false, bool is_legal = false> bool pos_moves(const game::Position &pos, movelist::MoveList &ml) {
    const auto turn = pos.turn();
    const auto opp = change_turn(turn);
    const auto move_flag = (turn == BLACK) ? (BLACK_FLAG | COLOR_WALL_FLAG) : (WHITE_FLAG | COLOR_WALL_FLAG);
    {
        const auto size = pos.piece_list_size(turn, PAWN);
        REP(index, size) {
            const auto from = pos.piece_list(turn, PAWN, index);
            const auto to = (turn == BLACK) ? from + INC_UP : from + INC_DOWN;
            if (attack::can_move(pos.square(to), move_flag)) {
                if (is_legal && attack::is_discover(from,to,turn,pos)) { continue; }
                if (is_exists) { return true; }
                ml.add(move(from, to, sq_is_prom(to, turn)));
            }
        }
    }

#define ADD_MOVE_PROM(dir) do {\
            const auto to = from + (dir);\
            if (attack::can_move(pos.square(to), move_flag)) {\
                if (is_legal && attack::is_discover(from,to,turn,pos)) { continue; }\
                if (is_exists) { return true; }\
                if (sq_is_prom(to, turn) || sq_is_prom(from, turn)) {\
                    ml.add(move(from, to, true));\
                }\
                ml.add(move(from, to));\
            }\
}while(false)
#define ADD_MOVE(dir) do {\
            const auto to = from + (dir);\
            if (attack::can_move(pos.square(to), move_flag)) {\
                if (is_legal && attack::is_discover(from,to,turn,pos)) { continue; }\
                if (is_exists) { return true; }\
                ml.add(move(from, to));\
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
                if (is_legal && attack::is_attacked(pos,to,opp)) { continue; }\
                if (is_exists) { return true; }\
                ml.add(move(from, to));\
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
            const auto inc = dir_to_inc(dir);\
            const auto neighbor_sq = pos.neighbor(from, dir);\
            auto to = from + inc;\
            for (; to != neighbor_sq; to += inc) {\
                if (is_legal && attack::is_discover(from,to,turn,pos)) { continue; }\
                if (is_exists) { return true; }\
                ASSERT2(to >= 0 && to < SQ_END, { Tee<<"error add_move_slider\n"; });\
                ml.add(move(from,to,(sq_is_prom(to, turn) || sq_is_prom(from, turn))));\
            }\
            if (attack::can_move(pos.square(to),move_flag)) {\
                if (is_legal && attack::is_discover(from,to,turn,pos)) { continue; }\
                if (is_exists) { return true; }\
                ASSERT2(to >= 0 && to < SQ_END, { Tee<<"error add_move_slider2\n"; });\
                ml.add(move(from,to,(sq_is_prom(to, turn) || sq_is_prom(from, turn))));\
            }\
}while(false)
#define ADD_MOVE_SLIER(dir) do {\
            const auto inc = dir_to_inc(dir);\
            const auto neighbor_sq = pos.neighbor(from, dir);\
            auto to = from + inc;\
            for (; to != neighbor_sq; to += inc) {\
                if (is_legal && attack::is_discover(from,to,turn,pos)) { continue; }\
                if (is_exists) { return true; }\
                ASSERT2(to >= 0 && to < SQ_END, { Tee<<"error add_move_slider\n"; });\
                ml.add(move(from,to));\
            }\
            if (attack::can_move(pos.square(to),move_flag)) {\
                if (is_legal && attack::is_discover(from,to,turn,pos)) { continue; }\
                if (is_exists) { return true; }\
                ASSERT2(to >= 0 && to < SQ_END, { Tee<<"error add_move_slider2\n"; });\
                ml.add(move(from,to));\
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
            ADD_MOVE(INC_UP);
            ADD_MOVE(INC_DOWN);
            ADD_MOVE(INC_LEFT);
            ADD_MOVE(INC_RIGHT);
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
            ADD_MOVE(INC_LEFTUP);
            ADD_MOVE(INC_LEFTDOWN);
            ADD_MOVE(INC_RIGHTUP);
            ADD_MOVE(INC_RIGHTDOWN);
        }
    }

#undef ADD_MOVE_SLIER_PROM
#undef ADD_MOVE_SLIER
#undef ADD_MOVE

    return false;
}

}
#endif