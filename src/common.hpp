#ifndef COMMON_HPP
#define COMMON_HPP

#include "libmy.hpp"
#include "overloadEnumOperators.hpp"

constexpr int FILE_SIZE{ 5 };
constexpr int RANK_SIZE{ 5 };
constexpr int SQUARE_SIZE{ FILE_SIZE * RANK_SIZE };
constexpr int SIDE_SIZE{ 2 };
constexpr int PIECE_SIZE{ 16 };
constexpr int PIECE_SIDE_SIZE{ 32 }; // excludes Empty #

// types

enum Square : int {
	SQ_11 = 0, SQ_12, SQ_13, SQ_14, SQ_15,
	SQ_21, SQ_22, SQ_23, SQ_24, SQ_25,
	SQ_31, SQ_32, SQ_33, SQ_34, SQ_35, 
	SQ_41, SQ_42, SQ_43, SQ_44, SQ_45,
	SQ_51, SQ_52, SQ_53, SQ_54, SQ_55, SQ_NONE = -1,
	SQ_FILE_INC = FILE_SIZE, SQ_RANK_INC = 1,
};

enum File : int { File_1, File_2, File_3, File_4, File_5, };
enum Rank : int { Rank_1, Rank_2, Rank_3, Rank_4, Rank_5, };
enum Side : int { BLACK, WHITE };

enum Piece : int {
	PieceNone, Pawn,  Silver,  Bishop,  Rook,  Gold, King,  DUMMY1, 
	DUMMY2,    PPawn, PSilver, PBishop, PRook, PieceProm = 8, Golds = 13,
};

enum PieceSide : int { Empty = PIECE_SIDE_SIZE, PieceSideProm = 8, };

enum Inc : int {
	Inc_N = -1,
	Inc_S = 1,
	Inc_E = -FILE_SIZE,
	Inc_W = FILE_SIZE,
	Inc_SW = Inc_S + Inc_W,
	Inc_NW = Inc_N + Inc_W,
	Inc_NE = Inc_N + Inc_E,
	Inc_SE = Inc_S + Inc_E,
};

enum Direction : int { DIR_UP, DIR_UL, DIR_LF, DIR_DL, DIR_DW, DIR_DR, DIR_RG, DIR_UR, DIR_L_KNT, DIR_R_KNT, DIR_NONE, };


const std::string PieceChar =     " . P S B R G K D D+P+S+B+R         ";
const std::string PieceSfenChar = " . P S B R G K D D+P+S+B+R       . p s b r g k d d+p+s+b+r         ";
const std::string SideChar = "bw";

inline std::string piece_to_sfen(const Piece pc) {
	return PieceChar.substr(pc * 2, 2);
}

inline std::string piece_side_to_sfen(const PieceSide pc) {
	return PieceSfenChar.substr(pc * 2, 2);
}

inline Piece sfen_to_piece(const std::string s) {
	return Piece(PieceChar.find(s) / 2);
}

inline PieceSide sfen_to_piece_side(const std::string s) {
	return PieceSide(PieceSfenChar.find(s) / 2);
}

inline bool piece_is_ok(const Piece p) {
	return (p >= Pawn) && (p <= PRook);
}

inline Piece piece_prom(const Piece p) {
	assert(p == Pawn || p == Silver || p == Bishop || p == Rook);
	return Piece(p | PieceProm);
}
inline Piece piece_unprom(const Piece p) {
	return Piece(p & (PieceProm - 1));
}
inline PieceSide piece_side_prom(const PieceSide p) {
	return PieceSide(p + PieceSideProm);
}
inline PieceSide piece_side_unprom(const PieceSide p) {
	return PieceSide(p - PieceSideProm);
}

inline Piece piece_side_piece(const PieceSide p) {
	return Piece(p & 0xf);
}

inline Side piece_side_side(const PieceSide p) {
	return Side((p >> 4) & 1);
}

inline PieceSide piece_side_make(const Piece p, const Side sd) {
	return PieceSide(p | (sd << 4));
}

inline constexpr Side flip_turn(const Side sd) {
	return Side(sd ^ 1);
}

inline constexpr bool piece_can_prom(const Piece p) {
	switch(p) {
		case Pawn:
		case Silver:
		case Bishop:
		case Rook:
			return true;
		case Gold:
		case King:
		case PPawn:
		case PSilver:
		case PBishop:
		case PRook:
			return false;		
		default:
			assert(false);
	}
}

OverloadEnumOperators(Square)
OverloadEnumOperators(File)
OverloadEnumOperators(Rank)
OverloadEnumOperators(Side)
OverloadEnumOperators(Piece)
OverloadEnumOperators(PieceSide)


enum class Key : uint64;
enum class Move : uint32;

enum Depth : int;
enum  Ply : int;
enum Score : int;

typedef float UCTScore;

OverloadEnumOperators(Depth)
OverloadEnumOperators(Ply)
OverloadEnumOperators(Score)


inline Key& operator ^= (Key& lhs, const Key rhs) { return lhs = static_cast<Key>(static_cast<uint64>(lhs) ^ static_cast<uint64>(rhs)); }

OverloadEnumOperators(Key)

inline constexpr Square square_make(const File f, const Rank r) {
	return Square(r + f * FILE_SIZE);
}

inline constexpr Rank square_rank(const Square sq) {
	return Rank(sq % RANK_SIZE);
}

template<Side sd> Rank side_rank(const Rank r) {
	return (sd == BLACK) ? r : (Rank_5 - r);
}

inline constexpr File square_file(const Square sq) {
	return File(sq / FILE_SIZE);
}

inline constexpr bool is_valid_file(const int f) {
	return (f >= File_1 && f < FILE_SIZE);
}
inline constexpr bool is_valid_rank(const int r) {
	return (r >= Rank_1 && r < RANK_SIZE);
}
inline constexpr bool is_valid(const int f, const int r) {
	return (is_valid_rank(r)) && (is_valid_file(f));
}
inline constexpr bool is_valid_sq(const int sq) {
	return (sq >= 0) && (sq < SQUARE_SIZE);
}

template<Side sd> bool square_is_prom(const Square sq) {
	const auto rank = square_rank(sq);
	return (sd == BLACK) ? (rank == Rank_1) : (rank == Rank_5);
}
bool square_is_prom(const Side sd, const Square sq);
inline constexpr Square flip_sq(const Square sq) {
	return Square(SQUARE_SIZE - sq - 1);
}

Square sq_from_string(const std::string& s);
std::string sq_to_string(const Square sq);

#define SQUARE_FOREACH(sq) for(auto sq = SQ_11; sq < SQUARE_SIZE; ++sq) 
#define SIDE_FOREACH(sd) for(auto sd = BLACK; sd < SIDE_SIZE; ++sd)
#define RANK_FOREACH(r) for(auto r = Rank_1; r < RANK_SIZE; ++r)
#define FILE_FOREACH(f) for(auto f = File_1; f < FILE_SIZE; ++f)
#define FILE_FOREACH_REV(f) for(auto f = File_5; f >= File_1; --f)
#define PIECE_FOREACH(p) for(auto p = Pawn; p <= PRook; ++p)
#define PIECE_SIDE_FOREACH(p) for(auto p = PieceSide(0); p < PieceSide(PIECE_SIDE_SIZE); ++p)

#define BP do{ std::cout<<__LINE__<<std::endl;  std::cin.get(); }while(0) 

extern Direction gDirection[SQUARE_SIZE][SQUARE_SIZE];

inline Direction get_direction(const Square from, const Square to) {
	return gDirection[from][to];
}

namespace common {

	void init();

}

#endif


