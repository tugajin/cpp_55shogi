#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "util.hpp"
/*
0	1	2	3	4	5	6	7
8	9	10v	11v	12v	13v	14v	15
16	17	18v	19v	20v	21v	22v	23
24	25	26v	27v	28v	29v	30v	31
32	33	34v	35v	36v	37v	38v	39
40	41	42v	43v	44v	45v	46v	47
48	49	50	51	52	53	54	55
56	57	58	59	60	61	62	63
*/


constexpr inline int SQUARE_SIZE = 25;
constexpr inline int DIR_SIZE = 8;

#define REP(i,e) for (auto i = 0; (i) < (e); ++(i))
#define REP_FILE(i) for (auto i = FILE_5; (i) < FILE_SIZE; ++(i)) 
#define REP_RANK(i) for (auto i = RANK_1; (i) < RANK_SIZE; ++(i)) 
#define REP_COLOR(i) for (auto i = BLACK; (i) < COLOR_SIZE; ++(i)) 
#define REP_PIECE(i) for (auto i = PAWN; (i) <= PROOK; ++(i)) 

typedef uint64 Key;

enum Move : int {
    MOVE_NONE = -1
};

enum Color : int {
    BLACK = 0, WHITE = 1, COLOR_SIZE = 2, COLOR_NONE = 3,
};

enum File : int {
    FILE_1 = 4, 
    FILE_2 = 3, 
    FILE_3 = 2, 
    FILE_4 = 1, 
    FILE_5 = 0, 
    FILE_SIZE = 5,
};

enum Rank : int {
    RANK_1 = 0,
    RANK_2 = 1,
    RANK_3 = 2,
    RANK_4 = 3,
    RANK_5 = 4,
    RANK_SIZE = 5,
};

enum Square : int {
    SQ_BEGIN = 0,
    SQ_11 = 14,
    SQ_12 = 22,
    SQ_13 = 30,
    SQ_14 = 38,
    SQ_15 = 46,
    SQ_21 = 13,
    SQ_22 = 21,
    SQ_23 = 29,
    SQ_24 = 37,
    SQ_25 = 45,
    SQ_31 = 12,
    SQ_32 = 20,
    SQ_33 = 28,
    SQ_34 = 36,
    SQ_35 = 44,
    SQ_41 = 11,
    SQ_42 = 19,
    SQ_43 = 27,
    SQ_44 = 35,
    SQ_45 = 43,
    SQ_51 = 10,
    SQ_52 = 18,
    SQ_53 = 26,
    SQ_54 = 34,
    SQ_55 = 42,
    SQ_END = 64,
    SQ_WALL = -1,
    INC_UP = -8,
    INC_DOWN = 8,
    INC_LEFT = -1,
    INC_RIGHT = 1,
    INC_LEFTUP = -9,
    INC_RIGHTUP = -7,
    INC_LEFTDOWN = 7,
    INC_RIGHTDOWN = 9,
    INC_NONE = 0,
};

static constexpr int INC_OFFSET = INC_RIGHTDOWN;
static constexpr int INC_MAX = 1 + INC_OFFSET * 2;

enum Piece : int {
    PROM_FLAG = 1 << 3,
    EMPTY = 0,
    PAWN = 1,
    SILVER = 2,
    BISHOP = 3,
    ROOK = 4,
    GOLD = 5,
    KING = 6, 
    PPAWN =   PAWN | PROM_FLAG,
    PSILVER = SILVER | PROM_FLAG,
    PBISHOP = BISHOP | PROM_FLAG,
    PROOK =   ROOK | PROM_FLAG,
    PIECE_END = 16,
};

enum ColorPiece : int {
    BLACK_FLAG = 1 << 4,
    WHITE_FLAG = 1 << 5,
    COLOR_WALL_FLAG = 1 << 6,
    UP_FLAG = 1 << 7,
    DOWN_FLAG = 1 << 8,
    LEFT_FLAG = 1 << 9,
    RIGHT_FLAG = 1 << 10,
    LEFTUP_FLAG = 1 << 11,
    LEFTDOWN_FLAG = 1 << 12,
    RIGHTUP_FLAG = 1 << 13,
    RIGHTDOWN_FLAG = 1 << 14,
    SLIDER_UP_FLAG = 1 << 15,
    SLIDER_DOWN_FLAG = 1 << 16,
    SLIDER_LEFT_FLAG = 1 << 17,
    SLIDER_RIGHT_FLAG = 1 << 18,
    SLIDER_LEFTUP_FLAG = 1 << 19,
    SLIDER_LEFTDOWN_FLAG = 1 << 20,
    SLIDER_RIGHTUP_FLAG = 1 << 21,
    SLIDER_RIGHTDOWN_FLAG = 1 << 22,
    BLACK_PAWN     = PAWN | BLACK_FLAG | UP_FLAG, 
    BLACK_SILVER   = SILVER | BLACK_FLAG | UP_FLAG | LEFTUP_FLAG | RIGHTUP_FLAG | LEFTDOWN_FLAG | RIGHTDOWN_FLAG, 
    BLACK_GOLDS    = BLACK_FLAG | UP_FLAG | LEFTUP_FLAG | RIGHTUP_FLAG | LEFT_FLAG | RIGHT_FLAG | DOWN_FLAG,
    BLACK_GOLD     = GOLD | BLACK_GOLDS, 
    BLACK_KING     = KING | BLACK_FLAG | UP_FLAG | LEFTUP_FLAG | RIGHTUP_FLAG | LEFT_FLAG | RIGHT_FLAG | LEFTDOWN_FLAG | RIGHTDOWN_FLAG | DOWN_FLAG,
    BLACK_BISHOP   = BISHOP | BLACK_FLAG | SLIDER_LEFTUP_FLAG | SLIDER_LEFTDOWN_FLAG | SLIDER_RIGHTUP_FLAG | SLIDER_RIGHTDOWN_FLAG,
    BLACK_ROOK     = ROOK | BLACK_FLAG | SLIDER_LEFT_FLAG | SLIDER_RIGHT_FLAG | SLIDER_UP_FLAG | SLIDER_DOWN_FLAG,
    BLACK_PPAWN    = PPAWN | BLACK_GOLDS,
    BLACK_PSILVER  = PSILVER | BLACK_GOLDS,
    BLACK_PBISHOP  = PBISHOP | BLACK_BISHOP | UP_FLAG | LEFT_FLAG | DOWN_FLAG | RIGHT_FLAG,
    BLACK_PROOK    = PROOK | BLACK_ROOK | LEFTUP_FLAG | LEFTDOWN_FLAG | RIGHTUP_FLAG | RIGHTDOWN_FLAG,
    WHITE_PAWN     = PAWN | WHITE_FLAG | DOWN_FLAG, 
    WHITE_SILVER   = SILVER | WHITE_FLAG | DOWN_FLAG | LEFTUP_FLAG | RIGHTUP_FLAG | LEFTDOWN_FLAG | RIGHTDOWN_FLAG, 
    WHITE_GOLDS    = WHITE_FLAG | UP_FLAG | LEFTDOWN_FLAG | RIGHTDOWN_FLAG | LEFT_FLAG | RIGHT_FLAG | DOWN_FLAG,
    WHITE_GOLD     = GOLD | WHITE_GOLDS, 
    WHITE_KING     = KING | WHITE_FLAG | UP_FLAG | LEFTUP_FLAG | RIGHTUP_FLAG | LEFT_FLAG | RIGHT_FLAG | LEFTDOWN_FLAG | RIGHTDOWN_FLAG | DOWN_FLAG,
    WHITE_BISHOP   = BISHOP | WHITE_FLAG | SLIDER_LEFTUP_FLAG | SLIDER_LEFTDOWN_FLAG | SLIDER_RIGHTUP_FLAG | SLIDER_RIGHTDOWN_FLAG,
    WHITE_ROOK     = ROOK | WHITE_FLAG | SLIDER_LEFT_FLAG | SLIDER_RIGHT_FLAG | SLIDER_UP_FLAG | SLIDER_DOWN_FLAG,
    WHITE_PPAWN    = PPAWN | WHITE_GOLDS,
    WHITE_PSILVER  = PSILVER | WHITE_GOLDS,
    WHITE_PBISHOP  = PBISHOP | WHITE_BISHOP | UP_FLAG | LEFT_FLAG | DOWN_FLAG | RIGHT_FLAG,
    WHITE_PROOK    = PROOK | WHITE_ROOK | LEFTUP_FLAG | LEFTDOWN_FLAG | RIGHTUP_FLAG | RIGHTDOWN_FLAG,
    COLOR_EMPTY = EMPTY,
    COLOR_WALL = COLOR_WALL_FLAG,
    COLOR_PIECE_END = (SLIDER_RIGHTDOWN_FLAG << 1) - 1,

    CEM = COLOR_EMPTY,//短縮形
    BPA = BLACK_PAWN,
    BSI = BLACK_SILVER,
    BBI = BLACK_BISHOP,
    BRO = BLACK_ROOK,
    BGO = BLACK_GOLD,
    BKI = BLACK_KING,
    BPP = BLACK_PAWN,
    BPS = BLACK_SILVER,
    BPB = BLACK_BISHOP,
    BPR = BLACK_ROOK,
    WPA = WHITE_PAWN,
    WSI = WHITE_SILVER,
    WBI = WHITE_BISHOP,
    WRO = WHITE_ROOK,
    WGO = WHITE_GOLD,
    WKI = WHITE_KING,
    WPP = WHITE_PAWN,
    WPS = WHITE_SILVER,
    WPB = WHITE_BISHOP,
    WPR = WHITE_ROOK,
};
/*
    .... .... .... ..xx pawn
    .... .... ...x x... silver
    .... .... xx.. .... bishop
    .... .xx. .... .... rook
    ..xx .... .... .... gold
    
*/
enum Hand : int { 
    HAND_NONE = 0,
    HAND_PAWN = 1 << 0,
    HAND_SILVER = 1 << 3,
    HAND_BISHOP = 1 << 6,
    HAND_ROOK = 1 << 9,
    HAND_GOLD = 1 << 12,
};

Color& operator++(Color& org) {
  org = static_cast<Color>(org + 1);
  return org;
}

Piece& operator++(Piece& org) {
  org = static_cast<Piece>(org + 1);
  return org;
}
File& operator++(File& org) {
  org = static_cast<File>(org + 1);
  return org;
}
Rank& operator++(Rank& org) {
  org = static_cast<Rank>(org + 1);
  return org;
}

Square operator+(Square l, Square r) {
  return static_cast<Square>(static_cast<int>(l) + static_cast<int>(r));
}
Square operator-(Square l, Square r) {
  return static_cast<Square>(static_cast<int>(l) - static_cast<int>(r));
}
Square& operator += (Square& lhs, const int rhs) { 
    return lhs  = static_cast<Square>(static_cast<int>(lhs) + rhs); 
} 
Square& operator += (Square& lhs, const Square rhs) { 
    return lhs += static_cast<int>(rhs); 
} 

constexpr inline int HAND_SHIFT[] = {0, 0, 3, 6, 9, 12};
constexpr inline int HAND_INC[] = {0, HAND_PAWN, HAND_SILVER, HAND_BISHOP, HAND_ROOK, HAND_GOLD };
constexpr inline int HAND_MASK[] = {0, 0x3 << 0, 0x3 << 3, 0x3 << 6, 0x3 << 9, 0x3 << 12 };
constexpr inline Square SQUARE_INDEX[] = { 
    SQ_51, SQ_41, SQ_31, SQ_21, SQ_11,
    SQ_52, SQ_42, SQ_32, SQ_22, SQ_12,
    SQ_53, SQ_43, SQ_33, SQ_23, SQ_13,
    SQ_54, SQ_44, SQ_34, SQ_24, SQ_14, 
    SQ_55, SQ_45, SQ_35, SQ_25, SQ_15, 
    SQ_WALL
};

extern int g_piece_color_piece[COLOR_SIZE][PIECE_END];

static constexpr inline int DELTA_OFFSET = 36;
static constexpr inline int DELTA_NB = DELTA_OFFSET * 2 + 1;

extern Square g_delta_inc_line[DELTA_NB];
extern ColorPiece g_delta_flag_all[DELTA_NB];
extern ColorPiece g_delta_flag_slider[DELTA_NB];

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
inline bool pseudo_attack(const ColorPiece p, const Square delta) {
    return (delta_flag_all(delta) & p) != 0;
}

inline bool pseudo_slider_attack(const ColorPiece p, const Square delta) {
    return (delta_flag_slider(delta) & p) != 0;
}

inline std::string color_str(const Color c) {
    return c == BLACK ? "先手" : "後手";
}
std::ostream& operator<<(std::ostream& os, const Color c) {
    os << color_str(c);
    return os;
}
inline constexpr Color change_turn(const Color turn) {
    return static_cast<Color>(static_cast<int>(turn) ^ 1);
}

inline constexpr Square square(const File f, const Rank r) {
    return static_cast<Square>(f + (r * 8) + 10);
}

inline constexpr Rank sq_rank(const Square sq) {
    return static_cast<Rank>((sq-10) >> 3);
}

inline constexpr File sq_file(const Square sq) {
    return static_cast<File>((sq-10) & 7);
}

inline Square invert_sq(const Square sq) {
    return Square(46) - sq;
}

inline Square mirror_sq(const Square sq) {
    auto file = sq_file(sq);
    auto rank = sq_rank(sq);
    file = static_cast<File>(static_cast<int>(FILE_SIZE) - 1 - static_cast<int>(file));
    return square(file, rank);
}

inline int sq_to_index_debug(const Square sq) {
   switch(sq) {
        case SQ_51: return 0;
        case SQ_41: return 1;
        case SQ_31: return 2;
        case SQ_21: return 3;
        case SQ_11: return 4;
        case SQ_52: return 5;
        case SQ_42: return 6;
        case SQ_32: return 7;
        case SQ_22: return 8;
        case SQ_12: return 9;
        case SQ_53: return 10;
        case SQ_43: return 11;
        case SQ_33: return 12;
        case SQ_23: return 13;
        case SQ_13: return 14;
        case SQ_54: return 15;
        case SQ_44: return 16;
        case SQ_34: return 17;
        case SQ_24: return 18;
        case SQ_14: return 19;
        case SQ_55: return 20;
        case SQ_45: return 21;
        case SQ_35: return 22;
        case SQ_25: return 23;
        case SQ_15: return 24;
        default: return -1;
    }
}
inline int sq_to_index(const Square sq) {
    const auto f = sq_file(sq);
    const auto r = sq_rank(sq);
    //https://www.wdic.org/w/TECH/%E3%83%93%E3%83%83%E3%83%88%E3%82%B7%E3%83%95%E3%83%88
    const auto index = f + (r<<2) + r;// f + r*3
    return index;
}

inline constexpr Hand inc_hand(const Hand h, const Piece p) {
    ASSERT2(p == PAWN || p == SILVER || p == GOLD || p == BISHOP || p == ROOK,{
        Tee<<p<<std::endl;
    });
    return static_cast<Hand>(h + HAND_INC[p]);
}

inline constexpr Hand dec_hand(const Hand h, const Piece p) {
    ASSERT(p == PAWN || p == SILVER || p == GOLD || p == BISHOP || p == ROOK);
    return static_cast<Hand>(h - HAND_INC[p]);
}

inline constexpr bool has_piece(const Hand h, const Piece p) {
    ASSERT(p == PAWN || p == SILVER || p == GOLD || p == BISHOP || p == ROOK);
    return (h & HAND_MASK[p]) != 0;
}

inline constexpr int num_piece(const Hand h, const Piece p) {
    ASSERT(p == PAWN || p == SILVER || p == GOLD || p == BISHOP || p == ROOK);
    return (h >> HAND_SHIFT[p]) & 0x3;
}

inline constexpr Move move(const Square from, const Square to, bool prom = false) {
    return static_cast<Move>(
        (static_cast<int>(prom) << 13) | (from << 6) | to
    );
}

inline constexpr Move move(const Square to, const Piece p) {
    return static_cast<Move>(
       (1 << 12) | (p << 6) | (to)
    );
}

inline constexpr Square move_from(const Move m) {
    return static_cast<Square>((m >> 6) & 0x3F);
}

inline constexpr Square move_to(const Move m) {
    return static_cast<Square>(m & 0x3F);
}

inline constexpr Piece move_piece(const Move m) {
    return static_cast<Piece>((m >> 6) & 0xF);
}

inline constexpr bool move_is_drop(const Move m) {
    return (m & (1 << 12)) != 0;
}

inline constexpr bool move_is_prom(const Move m) {
    return (m & (1 << 13)) != 0;
}

inline ColorPiece color_piece(const Piece p, const Color c) {
    return static_cast<ColorPiece>(g_piece_color_piece[c][p]);
}

inline constexpr Piece to_piece(const ColorPiece p) {
    return static_cast<Piece>(p & 0xF);
}

inline constexpr Piece prom(const Piece p) {
    ASSERT(p != GOLD);
    ASSERT(p != KING);
    ASSERT(p != EMPTY);
    return static_cast<Piece>(p | PROM_FLAG);
}

inline constexpr Piece unprom(const Piece p) {
    ASSERT(p != EMPTY);
    return static_cast<Piece>(p & (~PROM_FLAG));
}

inline constexpr Color piece_color(const ColorPiece p) {
    return static_cast<Color>((p >> 5) & 0x1);
}

inline ColorPiece prom(const ColorPiece cp) {
    const auto color = piece_color(cp);
    const auto pc = to_piece(cp);
    const auto ppc = prom(pc);
    return color_piece(ppc, color);
}

inline bool color_is_eq(const Color c, const ColorPiece cp) {
    static constexpr ColorPiece flag[] = { BLACK_FLAG, WHITE_FLAG };
    return (flag[c] & cp) != 0;
}

inline bool sq_is_ok(const Square sq) {
    for(auto *p = SQUARE_INDEX; *p != SQ_WALL; ++p) {
        if (*p == sq) {
            return true;
        }
    }
    return false;
}

inline bool sq_is_prom(const Square sq, const Color c) {
    const auto rank = sq_rank(sq);
    return (c == BLACK) ? (rank == RANK_1) : (rank == RANK_5);
}

inline bool piece_is_ok(const Piece p) {
    return (p == PAWN    || 
            p == SILVER  || 
            p == GOLD    || 
            p == BISHOP  || 
            p == ROOK    ||
            p == KING    ||
            p == PPAWN   || 
            p == PSILVER || 
            p == PBISHOP || 
            p == PROOK);
}

inline bool color_is_ok(const Color c) {
    return (c == BLACK || c == WHITE);
}

inline bool move_is_ok(const Move m) {
    if (move_is_drop(m)) {
        const auto piece = move_piece(m);
        const auto to = move_to(m);
        return piece_is_ok(piece) && sq_is_ok(to);
    } else {
        const auto from = move_from(m);
        const auto to = move_to(m);
        return sq_is_ok(from) && sq_is_ok(to);
    }
}

inline bool hand_is_ok(const Hand h) {
    static constexpr Hand HAND_MAX = static_cast<Hand>(18724);
    return h >= HAND_NONE && h <= HAND_MAX;
}

static inline std::string PIECE_STR[PIECE_END] = {"・","歩","銀","角","飛","金","玉","xx","xx","と","全","馬","龍"};

inline std::string piece_str(const Piece p) {
    return PIECE_STR[p];
}

inline std::string hand_str(const Hand h) {
    std::string ret = "";
    REP(i, num_piece(h, PAWN)) {
        ret += piece_str(PAWN);
    }
    REP(i, num_piece(h, SILVER)) {
        ret += piece_str(SILVER);
    }
    REP(i, num_piece(h, GOLD)) {
        ret += piece_str(GOLD);
    }
    REP(i, num_piece(h, BISHOP)) {
        ret += piece_str(BISHOP);
    }
    REP(i, num_piece(h, ROOK)) {
        ret += piece_str(ROOK);
    }
    if (ret == "") {
        return "持ち駒なし";
    } else {
        return ret;
    }
}


inline std::string sq_str(const Square sq) {
    static const std::string file_str[] = {"５","４","３","２","１"};
    static const std::string rank_str[] = {"一","二","三","四","五"};
    if (!sq_is_ok(sq)) {
        return "error_sq:" + to_string(int(sq));
    }
    const auto file = sq_file(sq);
    const auto rank = sq_rank(sq);
    return file_str[file] + rank_str[rank];
}

inline std::string move_str(const Move m) {
    std::string ret = "";
    if (m == MOVE_NONE) {
        ret += "MOVE_NONE";
    } else if (move_is_drop(m)) {
        ret +="★ " + sq_str(move_to(m))+piece_str(move_piece(m));
    } else {
        ret +=sq_str(move_from(m)) + sq_str(move_to(m));
        if (move_is_prom(m)) { ret += "成"; } 
    }
    ret += "(" + to_string(static_cast<int>(m)) + ")";
    return ret;
}

inline int color_piece_no(const ColorPiece p) {
    switch(p) {
        case COLOR_EMPTY: return 0;
        case BLACK_PAWN: return 1;
        case BLACK_SILVER: return 2;
        case BLACK_BISHOP: return 3;
        case BLACK_ROOK: return 4;
        case BLACK_GOLD: return 5;
        case BLACK_KING: return 6;
        case BLACK_PPAWN: return 7;
        case BLACK_PSILVER: return 8;
        case BLACK_PBISHOP: return 9;
        case BLACK_PROOK: return 10;
        case WHITE_PAWN: return 11;
        case WHITE_SILVER: return 12;
        case WHITE_BISHOP: return 13;
        case WHITE_ROOK: return 14;
        case WHITE_GOLD: return 15;
        case WHITE_KING: return 16;
        case WHITE_PPAWN: return 17;
        case WHITE_PSILVER: return 18;
        case WHITE_PBISHOP: return 19;
        case WHITE_PROOK: return 20;
        default: 
            Tee<<"error no:"<<p<<std::endl;
            return 0;
    }
}

inline ColorPiece piece_no_color_piece(const int index) {
    switch(index) {
        case 0: return COLOR_EMPTY;
        case 1: return BLACK_PAWN;
        case 2: return BLACK_SILVER;
        case 3: return BLACK_BISHOP;
        case 4: return BLACK_ROOK;
        case 5: return BLACK_GOLD;
        case 6: return BLACK_KING;
        case 7: return BLACK_PPAWN;
        case 8: return BLACK_PSILVER;
        case 9: return BLACK_PBISHOP;
        case 10: return BLACK_PROOK;
        case 11: return WHITE_PAWN;
        case 12: return WHITE_SILVER;
        case 13: return WHITE_BISHOP;
        case 14: return WHITE_ROOK;
        case 15: return WHITE_GOLD;
        case 16: return WHITE_KING;
        case 17: return WHITE_PPAWN;
        case 18: return WHITE_PSILVER;
        case 19: return WHITE_PBISHOP;
        case 20: return WHITE_PROOK;
        default: 
            Tee<<"error no2:"<<index<<std::endl;
            return COLOR_EMPTY;
    }
}

static constexpr Square DIR_INC[8] = { INC_UP, INC_LEFTUP, INC_LEFT, INC_LEFTDOWN, INC_DOWN, INC_RIGHTDOWN, INC_RIGHT, INC_RIGHTUP };
static constexpr int INC_DIR[INC_MAX] = { 1, 0, 7, -1, -1, -1, -1, -1, 2, -1, 6, -1, -1, -1, -1, -1, 3, 4, 5 };

inline constexpr Square dir_to_inc(const int dir) {
    ASSERT2(dir >=0 && dir < 8,{ Tee<<dir<<std::endl; });
    return DIR_INC[dir];
}

inline constexpr int inc_to_dir(const Square inc) {
    ASSERT2(inc+INC_OFFSET >=0 && inc+INC_OFFSET < INC_MAX,{ Tee<<inc<<std::endl; });
    return INC_DIR[inc+INC_OFFSET];
}

void check_mode() {
#if DEBUG
    Tee<<"debug mode\n";
#elif NDEBUG
    Tee<<"release mode\n";
#else
    Tee<<"unknown mode\n";
#endif
}


#endif