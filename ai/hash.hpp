#ifndef __HASH_HPP__
#define __HASH_HPP__
#include "game.hpp"

namespace hash {

extern Key g_hash_pos[COLOR_SIZE][PIECE_END][SQ_END];
extern Key g_hash_hand[COLOR_SIZE][PIECE_END][3];

static constexpr Key HASH_COLOR = Key(1);

//https://ja.wikipedia.org/wiki/Xorshift
// static関数とかstatic変数を使ってやばそうだが、cppファイルは1コなので多分大丈夫。
static uint32 xor128() {
    static uint32_t x = 123456789;
	static uint32_t y = 362436069;
	static uint32_t z = 521288629;
	static uint32_t w = 88675123;
	uint32_t t;

	t = x ^ (x << 11);
	x = y; y = z; z = w;
	return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

void init() {
    REP_COLOR(col) {
        REP(pc, PIECE_END) {
            g_hash_hand[col][pc][0] = xor128();
            g_hash_hand[col][pc][1] = xor128();
            g_hash_hand[col][pc][2] = xor128();
            REP(sq, SQ_END) {
                g_hash_pos[col][pc][sq] = xor128();
            }
        }
    }
}

Key hash_key(const game::Position &pos) {
    auto key = pos.turn() == BLACK ? Key(0) : HASH_COLOR;
    for(auto *p = SQUARE_INDEX; *p != SQ_WALL; ++p) {
        const auto sq = *p;
        const auto color_piece = pos.square(sq);
        if (color_piece != COLOR_EMPTY) {
            const auto color = piece_color(color_piece);
            const auto piece = to_piece(color_piece);
            key ^= g_hash_pos[color][piece][sq]; 
        }
    }
    REP_COLOR(col){
        for(auto num = 0; num <= num_piece(pos.hand(col),PAWN); num++) {
            key ^= g_hash_hand[col][PAWN][num];
        }
        for(auto num = 0; num <= num_piece(pos.hand(col),SILVER); num++) {
            key ^= g_hash_hand[col][SILVER][num];
        }
        for(auto num = 0; num <= num_piece(pos.hand(col),GOLD); num++) {
            key ^= g_hash_hand[col][GOLD][num];
        }
        for(auto num = 0; num <= num_piece(pos.hand(col),BISHOP); num++) {
            key ^= g_hash_hand[col][BISHOP][num];
        }
        for(auto num = 0; num <= num_piece(pos.hand(col),ROOK); num++) {
            key ^= g_hash_hand[col][ROOK][num];
        }
    }
    return key;
}

game::Position hirate() {
    return game::Position();
}

void test_hash() {
}
}
#endif