#ifndef __ATTACK_HPP__
#define __ATTACK_HPP__

#include "game.hpp"

namespace attack {

inline Square delta_inc_line(const Square delta) {
    ASSERT(delta + DELTA_OFFSET >= 0);
    ASSERT(delta + DELTA_OFFSET < DELTA_NB);
    return g_delta_inc_line[DELTA_OFFSET + delta];
}

inline ColorPiece delta_flag_all(const Square delta) {
    ASSERT2(delta + DELTA_OFFSET >= 0,{ Tee<<delta<<std::endl; });
    ASSERT2(delta + DELTA_OFFSET < DELTA_NB,{Tee<<delta<<std::endl;});
    return g_delta_flag_all[DELTA_OFFSET + delta];
}
inline ColorPiece delta_flag_slider(const Square delta) {
    ASSERT2(delta + DELTA_OFFSET >= 0,{ Tee<<delta<<std::endl; });
    ASSERT2(delta + DELTA_OFFSET < DELTA_NB,{Tee<<delta<<std::endl;});
    return g_delta_flag_slider[DELTA_OFFSET + delta];
}

inline bool line_is_empty(const Square from, const Square to, const game::Position &pos) {
    const auto inc = delta_inc_line(to - from);
    ASSERT(inc != INC_NONE);
    Square sq;
    for (sq = from + inc; sq != to; sq += inc) {
        if (pos.square(sq) != COLOR_EMPTY) {
            return false;
        }
    }
    ASSERT(sq == to);
    return true;
}

inline bool pseudo_attack(const ColorPiece p, const Square delta) {
    return (delta_flag_all(delta) & p) != 0;
}

inline bool pseudo_slider_attack(const ColorPiece p, const Square delta) {
    return (delta_flag_slider(delta) & p) != 0;
}

inline bool slider_attack(const Square from ,const Square to, const ColorPiece cp, const game::Position &pos) {
    const auto delta = to - from;
    return (pseudo_attack(cp, delta) && line_is_empty(from, to, pos));
}

class Checker {
public:
    int num;
    Square from[2];
    Square inc[2];
    //Bitboard self_pinner;
    //Bitboard enemy_pinner;
    Checker(const game::Position &pos) {
        this->from[0] = this->inc[0] = this->from[1] = this->inc[1] = SQ_END;
        this->num = 0;
        const auto turn = pos.turn();
        const auto opp = change_turn(turn);
        const auto color_flag = (opp == BLACK) ? BLACK_FLAG : WHITE_FLAG;
        const auto sq = pos.king_sq(turn);

#define SET_ATTACK(d, flag) do{\
        const auto from = sq + (d);\
        const auto color_piece = pos.square(from);\
        if ((color_piece & (flag)) == (flag)) {\
            this->from[this->num] = from;\
            this->inc[this->num] = INC_NONE;\
            ++this->num;\
            goto slider_phase;\
        }\
} while(false)
#define SET_SLIDER_ATTACK(sq_inc, flag) do{\
        const auto from = pos.neighbor(sq, inc_to_dir(sq_inc));\
        const auto color_piece = pos.square(from);\
        if ((color_piece & (flag)) == (flag)) {\
            this->from[this->num] = from;\
            this->inc[this->num] = (sq_inc);\
            ++this->num;\
            if (this->num == 2) { goto end; }\
        }\
} while(false)
        SET_ATTACK(INC_DOWN, UP_FLAG | color_flag);
        SET_ATTACK(INC_UP, DOWN_FLAG | color_flag);
        SET_ATTACK(INC_LEFT, RIGHT_FLAG | color_flag);
        SET_ATTACK(INC_RIGHT, LEFT_FLAG | color_flag);
        SET_ATTACK(INC_LEFTUP, RIGHTDOWN_FLAG | color_flag);
        SET_ATTACK(INC_LEFTDOWN, RIGHTUP_FLAG | color_flag);
        SET_ATTACK(INC_RIGHTUP, LEFTDOWN_FLAG | color_flag);
        SET_ATTACK(INC_RIGHTDOWN, LEFTUP_FLAG | color_flag);
slider_phase:
    // TODO is_attackedの書き方とどちらが速いのか。。。
        SET_SLIDER_ATTACK(INC_UP, SLIDER_DOWN_FLAG | color_flag);
        SET_SLIDER_ATTACK(INC_LEFTUP, SLIDER_RIGHTDOWN_FLAG | color_flag);
        SET_SLIDER_ATTACK(INC_LEFT, SLIDER_RIGHT_FLAG | color_flag);
        SET_SLIDER_ATTACK(INC_LEFTDOWN, SLIDER_RIGHTUP_FLAG | color_flag);
        SET_SLIDER_ATTACK(INC_DOWN, SLIDER_UP_FLAG | color_flag);
        SET_SLIDER_ATTACK(INC_RIGHTDOWN, SLIDER_LEFTUP_FLAG | color_flag);
        SET_SLIDER_ATTACK(INC_RIGHT, SLIDER_LEFT_FLAG | color_flag);
        SET_SLIDER_ATTACK(INC_RIGHTUP, SLIDER_LEFTDOWN_FLAG | color_flag);
end:;
#undef SET_ATTACK
#undef SET_SLIDER_ATTACK
    }
};


// class Attacker {
// public:
//     int attacker_piece_num[COLOR_SIZE][SQUARE_SIZE][PIECE_END];//最大の利きの数は8
//     int attacker_num[COLOR_SIZE][SQUARE_SIZE];
//     Attacker(const game::Position &pos) {
//         REP_COLOR(c) {
//             REP(sq, SQUARE_SIZE) {
//                 this->attacker_num[c][sq] = 0;
//                 REP(p, PIECE_END) {
//                     this->attacker_piece_num[c][sq][p] = 0;
//                 }
//             }
//         }
        
// #define ADD_HIYOKO(turn, dir) do {\
//         REP(i, pos.piece_list_size(turn, HIYOKO)) {\
//             const auto from = pos.piece_list(turn, HIYOKO,i);\
//             const auto to = from + dir;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][HIYOKO];\
//                 ++this->attacker_num[turn][index];\
//             }\
//         }\
//         }while(false)
// #define ADD_PIECE4(turn, piece, dir1,dir2,dir3,dir4) do {\
//         REP(i, pos.piece_list_size(turn, piece)) {\
//             const auto from = pos.piece_list(turn, piece,i);\
//             auto to = from + dir1;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][piece];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + dir2;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][piece];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + dir3;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][piece];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + dir4;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][piece];\
//                 ++this->attacker_num[turn][index];\
//             }\
//         }\
//         }while(false)
// #define ADD_NIWATORI(turn, dir1,dir2,dir3,dir4,dir5,dir6) do {\
//         REP(i, pos.piece_list_size(turn, NIWATORI)) {\
//             const auto from = pos.piece_list(turn, NIWATORI,i);\
//             auto to = from + dir1;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][NIWATORI];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + dir2;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][NIWATORI];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + dir3;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][NIWATORI];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + dir4;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][NIWATORI];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + dir5;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][NIWATORI];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + dir6;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][NIWATORI];\
//                 ++this->attacker_num[turn][index];\
//             }\
//         }\
//         }while(false)
// #define ADD_LION(turn) do {\
//         REP(i, pos.piece_list_size(turn, LION)) {\
//             const auto from = pos.piece_list(turn, LION,i);\
//             auto to = from + INC_LEFT;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][LION];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + INC_RIGHT;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][LION];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + INC_UP;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][LION];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + INC_DOWN;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][LION];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + INC_LEFTUP;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][LION];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + INC_RIGHTUP;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][LION];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + INC_LEFTDOWN;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][LION];\
//                 ++this->attacker_num[turn][index];\
//             }\
//             to = from + INC_RIGHTDOWN;\
//             if (pos.square(to) != COLOR_WALL) {\
//                 const auto index = sq_to_index(to);\
//                 ++this->attacker_piece_num[turn][index][LION];\
//                 ++this->attacker_num[turn][index];\
//             }\
//         }\
//         }while(false)
//         ADD_HIYOKO(BLACK, INC_UP);
//         ADD_HIYOKO(WHITE, INC_DOWN);
//         ADD_PIECE4(BLACK,ZOU,INC_LEFTUP, INC_LEFTDOWN, INC_RIGHTUP, INC_RIGHTDOWN);
//         ADD_PIECE4(WHITE,ZOU,INC_LEFTUP, INC_LEFTDOWN, INC_RIGHTUP, INC_RIGHTDOWN);
//         ADD_PIECE4(BLACK,KIRIN,INC_LEFT, INC_RIGHT, INC_UP, INC_DOWN);
//         ADD_PIECE4(WHITE,KIRIN,INC_LEFT, INC_RIGHT, INC_UP, INC_DOWN);
//         ADD_NIWATORI(BLACK, INC_LEFTUP, INC_UP, INC_RIGHTUP, INC_LEFT, INC_RIGHT, INC_DOWN);
//         ADD_NIWATORI(WHITE, INC_LEFTDOWN, INC_UP, INC_RIGHTDOWN, INC_LEFT, INC_RIGHT, INC_DOWN);
//         ADD_LION(BLACK);
//         ADD_LION(WHITE);

// #undef ADD_HIYOKO
// #undef ADD_PIECE4
// #undef ADD_NIWATORI
// #undef ADD_LION
//     }
//     std::string dump() const {
//         std::string str = "";
//         REP(i, SQUARE_SIZE) {
//             str += to_string(i) + ": bnum:" + to_string(this->attacker_num[BLACK][i]) + " bpiece: ";
//             REP_PIECE(p) {
//                 REP(num, this->attacker_piece_num[BLACK][i][p]) {
//                     str += piece_str(p);
//                 }
//             }
//             str += ": wnum:" + to_string(this->attacker_num[WHITE][i]) + " wpiece: ";
//             REP_PIECE(p) {
//                 REP(num, this->attacker_piece_num[WHITE][i][p]) {
//                     str += piece_str(p);
//                 }
//             }
//             str += "\n";
//         }
//         return str;
//     }
// };

inline bool is_attacked_debug(const game::Position &pos, const Square sq, const Color turn) {

#define CHECK_ATTACK(inc, flag) do{\
        const auto from = sq + (inc);\
        const auto color_piece = pos.square(from);\
        if ((color_piece & (flag)) == (flag)) {\
            return true;\
        }\
} while(false)

#define CHECK_SLIDER_ATTACK(inc, flag) do{\
        auto from = sq + (inc);\
        while (pos.square(from) == COLOR_EMPTY) { from += inc; };\
        const auto color_piece = pos.square(from);\
        if ((color_piece & (flag)) == (flag)) {\
            return true;\
        }\
} while(false)

    const auto color_flag = (turn == BLACK) ? BLACK_FLAG : WHITE_FLAG;

    CHECK_ATTACK(INC_DOWN, UP_FLAG | color_flag);
    CHECK_ATTACK(INC_UP, DOWN_FLAG | color_flag);
    CHECK_ATTACK(INC_LEFT, RIGHT_FLAG | color_flag);
    CHECK_ATTACK(INC_RIGHT, LEFT_FLAG | color_flag);
    CHECK_ATTACK(INC_LEFTUP, RIGHTDOWN_FLAG | color_flag);
    CHECK_ATTACK(INC_LEFTDOWN, RIGHTUP_FLAG | color_flag);
    CHECK_ATTACK(INC_RIGHTUP, LEFTDOWN_FLAG | color_flag);
    CHECK_ATTACK(INC_RIGHTDOWN, LEFTUP_FLAG | color_flag);

    CHECK_SLIDER_ATTACK(INC_DOWN, SLIDER_UP_FLAG | color_flag);
    CHECK_SLIDER_ATTACK(INC_UP, SLIDER_DOWN_FLAG | color_flag);
    CHECK_SLIDER_ATTACK(INC_LEFT, SLIDER_RIGHT_FLAG | color_flag);
    CHECK_SLIDER_ATTACK(INC_RIGHT, SLIDER_LEFT_FLAG | color_flag);
    CHECK_SLIDER_ATTACK(INC_LEFTUP, SLIDER_RIGHTDOWN_FLAG | color_flag);
    CHECK_SLIDER_ATTACK(INC_LEFTDOWN, SLIDER_RIGHTUP_FLAG | color_flag);
    CHECK_SLIDER_ATTACK(INC_RIGHTDOWN, SLIDER_LEFTUP_FLAG | color_flag);
    CHECK_SLIDER_ATTACK(INC_RIGHTUP, SLIDER_LEFTDOWN_FLAG | color_flag);

#undef CHECK_ATTACK
#undef CHECK_SLIDER_ATTACK

    return false;
}
inline bool is_attacked(const game::Position &pos, const Square sq, const Color turn) {

#define CHECK_ATTACK(inc, flag) do{\
        const auto from = sq + (inc);\
        const auto color_piece = pos.square(from);\
        if ((color_piece & (flag)) == (flag)) {\
            ASSERT(is_attacked_debug(pos,sq,turn));\
            return true;\
        }\
} while(false)

    const auto color_flag = (turn == BLACK) ? BLACK_FLAG : WHITE_FLAG;

    CHECK_ATTACK(INC_DOWN, UP_FLAG | color_flag);
    CHECK_ATTACK(INC_UP, DOWN_FLAG | color_flag);
    CHECK_ATTACK(INC_LEFT, RIGHT_FLAG | color_flag);
    CHECK_ATTACK(INC_RIGHT, LEFT_FLAG | color_flag);
    CHECK_ATTACK(INC_LEFTUP, RIGHTDOWN_FLAG | color_flag);
    CHECK_ATTACK(INC_LEFTDOWN, RIGHTUP_FLAG | color_flag);
    CHECK_ATTACK(INC_RIGHTUP, LEFTDOWN_FLAG | color_flag);
    CHECK_ATTACK(INC_RIGHTDOWN, LEFTUP_FLAG | color_flag);

#define CHECK_SLIDER_ATTACK(p)    do {\
        const auto size = pos.piece_list_size(turn, p);\
        REP(index, size) {\
            const auto from = pos.piece_list(turn, p, index);\
            const auto cp = color_piece(p, turn);\
            const auto inc = attack::delta_inc_line(sq - from);\
            if (inc == INC_NONE) { continue; }\
            const auto dir = inc_to_dir(inc);\
            const auto neighbor_sq = pos.neighbor(from, dir);\
            const auto neighbor_delta = neighbor_sq - from;\
            /*Tee<<"check_dir:"<<dir<<std::endl;\
            Tee<<"check_piece:"<<p<<std::endl;\
            Tee<<"from:"<<from<<std::endl;\
            Tee<<"sq:"<<sq<<std::endl;\
            Tee<<"nei:"<<neighbor_sq<<std::endl;\
            Tee<<"delta:"<<sq - from <<std::endl;\
            Tee<<"nei_delta:"<<neighbor_delta<<std::endl;\
            Tee<<"preudo_attack:"<<pseudo_slider_attack(cp, neighbor_delta)<<std::endl;\
            Tee<<"delta_flag:"<<delta_flag_slider(neighbor_delta)<<std::endl;*/\
            if (attack::pseudo_slider_attack(cp, neighbor_delta) && (std::abs(sq - from) <= std::abs(neighbor_delta))) {\
                return true;\
            }\
        }\
    }while (0)
    CHECK_SLIDER_ATTACK(BISHOP);
    CHECK_SLIDER_ATTACK(ROOK);
    CHECK_SLIDER_ATTACK(PBISHOP);
    CHECK_SLIDER_ATTACK(PROOK);
#undef CHECK_ATTACK
#undef CHECK_SLIDER_ATTACK
    ASSERT2(!is_attacked_debug(pos,sq,turn),{
        Tee<<"is_attacked_debug not eq error\n";
        Tee<<pos<<std::endl;
        pos.dump();
        Tee<<sq_str(sq)<<std::endl;

    });
    return false;
}

inline bool in_checked(const game::Position &pos) {
    return is_attacked(pos,pos.king_sq(pos.turn()),change_turn(pos.turn()));
}

inline bool can_move(const ColorPiece p, const int move_flag) {
    return (p & move_flag) == 0;
}

inline bool is_pinned(const Square sq, const Color me, const game::Position &pos) {
    const auto king_sq = pos.king_sq(me);
    // 玉とsqの間に何もないか？
    const auto inc_king = attack::delta_inc_line(king_sq - sq);
    if (inc_king == INC_NONE) { return false; }
    const auto dir_king = inc_to_dir(inc_king);
    const auto pseudo_king_sq = pos.neighbor(sq,dir_king);
    if (pseudo_king_sq != king_sq) { return false; }

    // sqと飛び駒の間に何もないか？
    const auto inc_slider = static_cast<Square>(-inc_king);
    const auto dir_slider = inc_to_dir(inc_slider);
    const auto slider_sq = pos.neighbor(sq, dir_slider);
    const auto slider_cp = pos.square(slider_sq);
    const auto opp = change_turn(me);
    return color_is_eq(opp,slider_cp) && attack::pseudo_slider_attack(slider_cp, sq - slider_sq);
}

inline bool is_discover(const Square from, const Square to, const Color turn, const game::Position &pos) {
    const auto king_sq = pos.king_sq(turn);
    if (is_pinned(from,turn, pos)) {
        return attack::delta_inc_line(king_sq - from) != attack::delta_inc_line(king_sq - to);
    }
    return false;
}

void init() {


    REP(i, 16) {
        g_piece_color_piece[BLACK][i] = g_piece_color_piece[WHITE][i] = -1;
    }

    g_piece_color_piece[BLACK][PAWN] = BLACK_PAWN;
    g_piece_color_piece[BLACK][SILVER] = BLACK_SILVER;
    g_piece_color_piece[BLACK][BISHOP] = BLACK_BISHOP;
    g_piece_color_piece[BLACK][ROOK] = BLACK_ROOK;
    g_piece_color_piece[BLACK][GOLD] = BLACK_GOLD;
    g_piece_color_piece[BLACK][KING] = BLACK_KING;
    g_piece_color_piece[BLACK][PPAWN] = BLACK_PPAWN;
    g_piece_color_piece[BLACK][PSILVER] = BLACK_PSILVER;
    g_piece_color_piece[BLACK][PBISHOP] = BLACK_PBISHOP;
    g_piece_color_piece[BLACK][PROOK] = BLACK_PROOK;

    g_piece_color_piece[WHITE][PAWN] = WHITE_PAWN;
    g_piece_color_piece[WHITE][SILVER] = WHITE_SILVER;
    g_piece_color_piece[WHITE][BISHOP] = WHITE_BISHOP;
    g_piece_color_piece[WHITE][ROOK] = WHITE_ROOK;
    g_piece_color_piece[WHITE][GOLD] = WHITE_GOLD;
    g_piece_color_piece[WHITE][KING] = WHITE_KING;
    g_piece_color_piece[WHITE][PPAWN] = WHITE_PPAWN;
    g_piece_color_piece[WHITE][PSILVER] = WHITE_PSILVER;
    g_piece_color_piece[WHITE][PBISHOP] = WHITE_PBISHOP;
    g_piece_color_piece[WHITE][PROOK] = WHITE_PROOK;

    REP(i, DELTA_NB) {
        g_delta_inc_line[i] = INC_NONE;
        g_delta_flag_all[i] = COLOR_EMPTY;
        g_delta_flag_slider[i] = COLOR_EMPTY;
    }
    const ColorPiece flags[] = { UP_FLAG, LEFTUP_FLAG, LEFT_FLAG, LEFTDOWN_FLAG, 
                                 DOWN_FLAG, RIGHTDOWN_FLAG, RIGHT_FLAG, RIGHTUP_FLAG }; 
    const ColorPiece slider_flags[] = { SLIDER_UP_FLAG, SLIDER_LEFTUP_FLAG, SLIDER_LEFT_FLAG, SLIDER_LEFTDOWN_FLAG, 
                                        SLIDER_DOWN_FLAG, SLIDER_RIGHTDOWN_FLAG, SLIDER_RIGHT_FLAG, SLIDER_RIGHTUP_FLAG }; 
    REP(dir, 8) {
        const auto inc = dir_to_inc(dir);
        auto delta = inc;
        g_delta_flag_all[DELTA_OFFSET + delta] = flags[dir];
        //壁までいくことがあるので、5回実施
        REP(num, 5){
            g_delta_flag_all[DELTA_OFFSET + delta] = static_cast<ColorPiece>(
                                                    static_cast<int>(g_delta_flag_all[DELTA_OFFSET + delta]) | 
                                                    static_cast<int>(slider_flags[dir])
                                                );
            g_delta_flag_slider[DELTA_OFFSET + delta] = slider_flags[dir];
            g_delta_inc_line[DELTA_OFFSET + delta] = inc;
            delta += inc;
        }
    }
    ASSERT(g_delta_flag_all[DELTA_OFFSET + INC_UP] == (UP_FLAG | SLIDER_UP_FLAG));
    ASSERT(g_delta_flag_all[DELTA_OFFSET + INC_DOWN] == (DOWN_FLAG | SLIDER_DOWN_FLAG));
    ASSERT(g_delta_flag_all[DELTA_OFFSET + INC_LEFTUP] == (LEFTUP_FLAG | SLIDER_LEFTUP_FLAG));
    ASSERT(g_delta_flag_all[DELTA_OFFSET + INC_LEFTDOWN] == (LEFTDOWN_FLAG | SLIDER_LEFTDOWN_FLAG));
    ASSERT(g_delta_flag_all[DELTA_OFFSET + INC_LEFT] == (LEFT_FLAG | SLIDER_LEFT_FLAG));
    ASSERT(g_delta_flag_all[DELTA_OFFSET + INC_RIGHT] == (RIGHT_FLAG | SLIDER_RIGHT_FLAG));
    ASSERT(g_delta_flag_all[DELTA_OFFSET + INC_RIGHTUP] == (RIGHTUP_FLAG | SLIDER_RIGHTUP_FLAG));
    ASSERT(g_delta_flag_all[DELTA_OFFSET + INC_RIGHTDOWN] == (RIGHTDOWN_FLAG| SLIDER_RIGHTDOWN_FLAG));
}

}
namespace game {
bool Position::is_win() const {
    const auto me = this->turn();
    const auto opp = change_turn(me);
    return attack::is_attacked(*this, this->king_sq(opp), me);
}
}
namespace gen {

bool is_mate_with_pawn_drop(const Square drop_sq, game::Position &pos) {
    const auto me = pos.turn();
    const auto opp = change_turn(me);
    const auto king_sq = (me == BLACK) ? drop_sq + INC_UP : drop_sq + INC_DOWN;
    const auto pawn = (me == BLACK) ? BLACK_PAWN : WHITE_PAWN;
    if (pos.king_sq(opp) != king_sq) {
        return false;
    }
    pos.next_quick(drop_sq, pawn);
    //capture attacker
    REP(dir, 8) {
        const auto from = pos.neighbor(drop_sq, dir);
        if (from == king_sq) {continue;}
        const auto from_cp = pos.square(from);
        if (color_is_eq(opp, from_cp) 
        && attack::pseudo_attack(from_cp, drop_sq - from) 
        && !attack::is_discover(from, drop_sq, opp, pos)) {
            pos.prev_quick(drop_sq);
            return false;
        }
    }
    const auto king = (me == BLACK) ? WHITE_KING : BLACK_KING;
    const auto move_flag = (me == BLACK) ? (WHITE_FLAG | COLOR_WALL_FLAG) : (BLACK_FLAG | COLOR_WALL_FLAG);
    //escape king
    pos.prev_quick(king_sq);
    REP(dir, 8) {
        const auto to = king_sq + dir_to_inc(dir);
        if (!attack::can_move(pos.square(to), move_flag)) { continue; }
        if (!attack::is_attacked(pos, to, me)) {
            pos.next_quick(king_sq, king);
            pos.prev_quick(drop_sq);
            return false;
        }
    }
    pos.next_quick(king_sq, king);
    pos.prev_quick(drop_sq);

#if 0
    const auto mv = move(drop_sq, PAWN);
    auto pos2 = pos.next(mv);
    if (!pos2.is_lose()) {
        Tee<<"mate with pawn drop error\n";
        Tee<<pos<<std::endl;
        ASSERT(false);
    }

#endif

    return true;
}
}
namespace attack {
inline void test_attack() {
    {
        auto pos = sfen::sfen("1+RKss/p4/5/bPgG1/4k w br - 80");
        Tee<<pos<<std::endl;
        ASSERT(!attack::in_checked(pos));
    }
    {
        auto pos = sfen::sfen("2k1p/2Grr/p3B/1S1+s1/K2gb w - 116");
        Tee<<pos<<std::endl;
        ASSERT(attack::is_pinned(SQ_22,WHITE,pos));
    }
    {
        auto pos = sfen::sfen("r2gk/4p/s1S2/PK2b/1G1BR b - 1");
        Tee<<pos<<std::endl;
        ASSERT(!attack::is_attacked(pos,SQ_34,WHITE));
    }
    {
        auto pos = sfen::hirate();
        pos = pos.next(move(SQ_15,SQ_12));
        Tee<<pos<<std::endl;
        ASSERT(attack::is_attacked(pos,SQ_22,BLACK));
    }
    {
        auto pos = sfen::sfen("r1s1k/b1g1B/5/P4/KGSR1 w P - 6");
        Tee<<pos<<std::endl;
        ASSERT(attack::is_attacked(pos,SQ_22,WHITE));
    }
    {
        auto pos = sfen::sfen("rbs1k/B2gp/5/P4/KGS1R b - 3");
        Tee<<pos<<std::endl;
        ASSERT(!attack::is_pinned(SQ_52,BLACK,pos));
    }
    {
        auto pos = sfen::sfen("rbsgk/4p/5/P4/KGSBR b - 1");
        Tee<<pos<<std::endl;
        ASSERT(!attack::is_attacked(pos,SQ_44,WHITE));
    }
    {
        auto pos = sfen::sfen("1rkpS/R4/5/5/K4 b P - 1");
        Tee<<pos<<std::endl;
        ASSERT(gen::is_mate_with_pawn_drop(SQ_32,pos));
    }
    {
        auto pos = sfen::sfen("4k/4r/5/4P/4K b p - 1");
        Tee<<pos<<std::endl;
        ASSERT(is_pinned(SQ_14,BLACK,pos));
    }
    {
        auto pos = sfen::sfen("1G2k/4p/r1g1s/5/bKB+sR w p - 28");
        Tee<<pos<<std::endl;
        ASSERT(!is_pinned(SQ_35,BLACK,pos));
        ASSERT(!gen::is_mate_with_pawn_drop(SQ_44,pos));
    }
    {
        auto pos = sfen::sfen("1b2k/4p/s1g2/1G3/KRSBR w p - 18");
        Tee<<pos<<std::endl;
        ASSERT(!gen::is_mate_with_pawn_drop(SQ_54,pos));
    }
    {
        auto pos = sfen::sfen("1b3/3kp/s1g1R/1G3/KRSB1 w p - 20");
        Tee<<pos<<std::endl;
        ASSERT(!gen::is_mate_with_pawn_drop(SQ_54,pos));
    }
    {
        auto pos = sfen::sfen("1b2k/4p/s1g2/1G3/KRSBR w p - 18");
        Tee<<pos<<std::endl;
        ASSERT(!gen::is_mate_with_pawn_drop(SQ_54,pos));
    }
    {
        auto pos = sfen::sfen("1b1gk/r1S2/1rG1p/2s2/K2B1 w p - 1");
        Tee<<pos<<std::endl;
        ASSERT(pos.is_win());
    }
    {
        auto pos = sfen::sfen("rb1gS/1s2R/3k1/PS3/K1GB1 b P - 7");
        Tee<<pos<<std::endl;
        ASSERT(!gen::is_mate_with_pawn_drop(SQ_24,pos));
    }
    {
        auto pos = sfen::sfen("rb1gS/2s1k/4p/PSB2/KG2R b - 1");
        Tee<<pos<<std::endl;
        Tee<<pos.is_win()<<std::endl;
    } 
    {
        auto pos = sfen::sfen("1gkpS/R4/5/5/K4 b P - 1");
        Tee<<pos<<std::endl;
        ASSERT(!gen::is_mate_with_pawn_drop(SQ_32,pos));
    }
    {
        auto pos = sfen::sfen("RgkpS/R4/5/5/K4 b P - 1");
        Tee<<pos<<std::endl;
        ASSERT(gen::is_mate_with_pawn_drop(SQ_32,pos));
    }
    {
        auto pos = sfen::sfen("RgkpS/R2g1/4B/5/K4 b P - 1");
        Tee<<pos<<std::endl;
        ASSERT(gen::is_mate_with_pawn_drop(SQ_32,pos));
    }
    {
        auto pos = sfen::sfen("RgkpS/R2g1/5/5/K4 b P - 1");
        Tee<<pos<<std::endl;
        ASSERT(!gen::is_mate_with_pawn_drop(SQ_32,pos));
    }
    {
        auto pos = sfen::sfen("Rgkp1/R4/5/5/K4 b P - 1");
        Tee<<pos<<std::endl;
        ASSERT(!gen::is_mate_with_pawn_drop(SQ_32,pos));
    }
}
}
#endif