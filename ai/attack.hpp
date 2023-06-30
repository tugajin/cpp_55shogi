#ifndef __ATTACK_HPP__
#define __ATTACK_HPP__

#include "game.hpp"

namespace attack {

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

inline bool is_attacked(const game::Position &pos, const Square sq, const Color turn) {
#define CHECK_ATTACK(inc, flag) do{\
        const auto from = sq + (inc);\
        const auto color_piece = pos.square(from);\
        if ((color_piece & (flag)) == (flag)) {\
            return true;\
        }\
} while(false)

#define CHECK_SLIDER_ATTACK(inc, flag) do{\
        const auto dir = inc_to_dir(inc);\
        const auto from = pos.neighbor(sq, dir);\
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
    CHECK_SLIDER_ATTACK(INC_RIGHTUP, SLIDER_LEFTDOWN_FLAG | color_flag);
    CHECK_SLIDER_ATTACK(INC_RIGHTDOWN, SLIDER_LEFTUP_FLAG | color_flag);

#undef CHECK_ATTACK
#undef CHECK_SLIDER_ATTACK

    return false;
}

inline bool in_checked(const game::Position &pos) {
    return is_attacked(pos,pos.king_sq(pos.turn()),change_turn(pos.turn()));
}

inline bool can_move(const ColorPiece p, const int move_flag) {
    return (p & move_flag) == 0;
}

inline void test_attack() {
    auto pos = sfen::sfen("rb1g1/2s1k/4p/PSB2/KG2R b - 1");
    Tee<<pos<<std::endl;
    Tee<<pos.is_win()<<std::endl;
}
}
namespace game {
bool Position::is_win() const {
    const auto me = this->turn();
    const auto opp = change_turn(me);
    return attack::is_attacked(*this, this->king_sq(opp), me);
}
}
#endif