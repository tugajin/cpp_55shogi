#include "game.hpp"

namespace sfen {

static const std::string SFEN_STR[COLOR_SIZE][PIECE_END] = {{"・","P","S","B","R","G","K","xx","xx","+P","+S","+B","+R","xx","xx","xx"},{"・","p","s","b","r","g","k","xx","xx","+p","+s","+b","+r","xx","xx","xx"}};
static const std::string START_SFEN = "rbsgk/4p/5/P4/KGSBR - 1";

ColorPiece from_sfen(const std::string &s) {
    REP_COLOR(col) {
        REP_PIECE(pc) {
            if (SFEN_STR[col][pc] == s) {
                return color_piece(pc,col);
            }
        }
    }
    return COLOR_EMPTY;
}
std::string to_sfen(const Piece pc, const Color col) {
    return SFEN_STR[col][pc];
}
std::string to_sfen(const ColorPiece cp) {
    const auto col = piece_color(cp);
    const auto pc = to_piece(cp);
    return to_sfen(pc, col);
}

game::Position sfen(std::string sfen) {
    ColorPiece pieces[SQUARE_SIZE] = {COLOR_EMPTY};
    Hand hands[COLOR_SIZE] = { HAND_NONE, HAND_NONE };
    auto pos = 0;
    //piece placement
    auto sq = 0;
    auto prom_flag = false;
    while (pos < int(sfen.size())) {
        auto c = sfen[pos++];
        if (false) {
        } else if (c == ' ') {
            break;
        } else if (c == '/') {
            continue;
        } else if (std::isdigit(c)) {
            sq += c - '0';
        } else if (c == '+') {
            prom_flag = true;
            continue;
        } else { // assume piece
            std::string pc_str = { c };
            ColorPiece cp = from_sfen(pc_str);
            if (prom_flag) {
                cp = prom(cp);
                prom_flag = false;
            }
            pieces[sq++] = cp;
        }
    }

    //turn

    auto turn = BLACK;

    if (pos < int(sfen.size())) {
        auto c = sfen[pos++];
        if (c == 'w') {
            turn = WHITE;
        }
        if (pos < int(sfen.size())) {
            assert(sfen[pos] == ' ');
            pos++;
        }
    }

    //hand
    auto num = 0;
    while (pos < int(sfen.size())) {
        auto c = sfen[pos++];
        if (false) {
        } else if (c == ' ') {
            break;
        } else if (c == '-') {
            break;
        } else if (std::isdigit(c)) {
            num = (num) ? 10 * num + int(c - '0') : int(c - '0');
        } else {
            std::string pc_str = { c };
            const ColorPiece cp = from_sfen(pc_str);
            const auto col = piece_color(cp);
            const auto pc = to_piece(cp);
            num = (num) ? num : 1;
            for (auto i = 0; i < num; i++) {
                hands[col] = inc_hand(hands[col],pc);
            }
            num = 0;
        }
    }
    return game::Position(pieces, hands, turn);
}

std::string out_sfen(const game::Position &pos) {
    std::string str = "sfen ";
    auto empty = 0;
    REP_RANK(rank) {
        REP_FILE(file) {
            const auto sq = square(file, rank);
            const auto cp = pos.square(sq);
            if(cp == COLOR_EMPTY) {
                empty++;
                continue;
            } else {
                if(empty) {
                    str += to_string(empty);
                    empty = 0;
                }
                str += to_sfen(cp);
            }
        }
        if(empty) {
            str += to_string(empty);
            empty = 0;
        }
        if(rank + 1 != RANK_SIZE) { str += "/"; }
    }
    str += (pos.turn() == BLACK) ? " b " : " w ";
    if(pos.hand(BLACK) != HAND_NONE || pos.hand(WHITE) != HAND_NONE) {
        REP_COLOR(col) {
            REP_PIECE(pc) {
                if (pc != PAWN && pc != SILVER && pc != GOLD && pc != BISHOP && pc != ROOK) {
                    continue;
                }
                const auto num = num_piece(pos.hand(col), pc);
                if(num) {
                    if(num > 1) {
                        str += to_string(num);
                    }
                    str += to_sfen(pc, col);
                }
            }
        }
        str += " ";
    }
    str += "- ";
    str += to_string(pos.ply()+1);
    return str;
}

game::Position hirate() {
    return sfen(START_SFEN);
}

void test_sfen() {
    auto pos = hirate();
    Tee<<pos<<std::endl;
}

}