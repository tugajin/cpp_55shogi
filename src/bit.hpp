#ifndef BIT_HPP
#define BIT_HPP

#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

#include "libmy.hpp"
#include "common.hpp"

#include <array>
#include <bitset>

namespace bit {

	//Apprry、やねうら王モロパクリ

	class Bitboard {
	private:
		uint64 p_;
	public:
		Bitboard() {}
		Bitboard(uint64 u0) {
			this->set(u0);
		}
		uint64 p() const {
			return this->p_;
		}
		Bitboard& operator = (const Bitboard& rhs) {
			p_ = rhs.p_;
			return *this;
		}
		Bitboard operator &=(const Bitboard& rhs) {
			p_ &= rhs.p_;
			return *this;
		}
		Bitboard operator |=(const Bitboard& rhs) {
			p_ |= rhs.p_;
			return *this;
		}
		Bitboard operator ^=(const Bitboard& rhs) {
			p_ ^= rhs.p_;
			return *this;
		}
		Bitboard operator <<=(const int i) {
			p_ = p_ << i;
			return *this;
		}
		Bitboard operator >>=(const int i) {
			p_ = p_ >> i;
			return *this;
		}
		Bitboard operator &(const Bitboard& rhs) const {
			return Bitboard(*this) &= rhs;
		}
		Bitboard operator |(const Bitboard& rhs) const {
			return Bitboard(*this) |= rhs;
		}
		Bitboard operator ^(const Bitboard& rhs) const {
			return Bitboard(*this) ^= rhs;
		}
		Bitboard operator <<(const int i) const {
			return Bitboard(*this) <<= i;
		}
		Bitboard operator >>(const int i) const {
			return Bitboard(*this) >>= i;
		}
		bool operator ==(const Bitboard& rhs) const {
			return p_ == rhs.p_;
		}
		bool operator !=(const Bitboard& rhs) const {
			return p_ != rhs.p_;
		}
		Bitboard& operator += (const Bitboard& b1) {
			this->p_ += b1.p_;
			return *this;
		}
		void set(const uint64 u0) {
			this->p_ = u0;
		}
		void init() {
			set(0ull);
		}
		explicit operator bool() const {
			return p_;
		}
		int pop_cnt() const {
			return ml::bit_count(this->p_);
		}
		template<bool is_del = true> Square lsb() {
			assert(this->p_ != 0ull);
			const auto ret = Square(ml::bit_first(p_));
			if(is_del) {
				p_ &= p_ - 1;
			}
			return ret;
		}
		friend std::ostream& operator<<(std::ostream& os, const Bitboard& b) {
			os << b.p_ << std::endl;
			os << "-----------------" << std::endl;
			for (int rank = 0; rank < 5; rank++) {
				for (int file = 4; file >= 0; file--) {
					int xy = rank + file * 5;
					(b.is_set(xy)) ? os << "1," : os << "0,";
				}
				os << std::endl;
			}
			return os;
		}
		bool is_set(const int) const;
		Bitboard operator &=(const int);
		Bitboard operator |=(const int);
		Bitboard operator ^=(const int);
		Bitboard operator &(const int xy) const {
			return Bitboard(*this) &= xy;
		}
		Bitboard operator |(const int xy) const {
			return Bitboard(*this) |= xy;
		}
		Bitboard operator ^(const int xy) const {
			return Bitboard(*this) ^= xy;
		}
		Bitboard operator + (const Bitboard& rhs) const {
			return Bitboard(*this) += rhs;
		}
		void clear(const Square sq) {
			assert(is_set(sq));
			(*this) ^= sq;
		}
		void set(const Square sq) {
			(*this) |= sq;
		}
	};

	extern std::array<Bitboard, SQUARE_SIZE> g_mask;
	extern std::array<Bitboard, SQUARE_SIZE> g_bishop_mask;
	extern std::array<Bitboard, SQUARE_SIZE> g_rook_mask;
	extern std::array<Bitboard, SQUARE_SIZE> g_file_mask;
	extern std::array<Bitboard, SQUARE_SIZE> g_rank_mask;

	extern std::array<Bitboard, SIDE_SIZE> g_prom; //1
	extern std::array<Bitboard, SIDE_SIZE> g_middle; //2~5
	extern Bitboard g_all_one_bb;
	extern Bitboard g_all_zero_bb;
	extern std::array<std::array<Bitboard, 1 << 5>, SIDE_SIZE> g_double_pawn_mask;

	typedef std::array<std::array<bit::Bitboard, SQUARE_SIZE>, SIDE_SIZE> bw_square_t;

	extern bw_square_t g_silver_attacks;
	extern bw_square_t g_gold_attacks;
	extern std::array<bit::Bitboard, 1856 + 1> g_bishop_attack;
	extern std::array<bit::Bitboard, 1856 + 1> g_rook_attack;
	extern std::array<int, SQUARE_SIZE> g_bishop_offset;
	extern std::array<int, SQUARE_SIZE> g_rook_offset;

	extern Bitboard gBetween[638];
	extern Bitboard gBeyond[393];

	extern int gBetweenIndex[SQUARE_SIZE][SQUARE_SIZE];
	extern int gBeyondIndex[SQUARE_SIZE][SQUARE_SIZE];

	extern bit::Bitboard g_check_area[SIDE_SIZE][PIECE_SIZE][SQUARE_SIZE];

	inline Bitboard Bitboard::operator &=(const int xy) {
		*this &= g_mask[xy];
		return *this;
	}

	inline Bitboard Bitboard::operator |=(const int xy) {
		*this |= g_mask[xy];
		return *this;
	}

	inline Bitboard Bitboard::operator ^=(const int xy) {
		*this ^= g_mask[xy];
		return *this;
	}


	inline bool Bitboard::is_set(const int sq) const {
		return (bool)((*this) & sq);
	}

	inline Bitboard operator ~ (const Bitboard& bb) { return bb ^ g_all_one_bb; }

	inline uint64 occ_to_index(const bit::Bitboard& bb, const bit::Bitboard& mask) {
		return ml::pext(bb.p(), mask.p());
	}

	inline bit::Bitboard get_bishop_attack(const Square sq, const bit::Bitboard& occ) {
		return bit::g_bishop_attack[bit::g_bishop_offset[sq]
			+ occ_to_index(occ & bit::g_bishop_mask[sq], bit::g_bishop_mask[sq])];
	}
	inline bit::Bitboard get_pseudo_bishop_attack(const Square sq) {
		return bit::g_bishop_attack[bit::g_bishop_offset[sq]];
	}
	inline bit::Bitboard get_rook_attack(const Square sq, const bit::Bitboard& occ) {
		return bit::g_rook_attack[bit::g_rook_offset[sq]
			+ occ_to_index(occ & bit::g_rook_mask[sq], bit::g_rook_mask[sq])];
	}
	inline bit::Bitboard get_pseudo_rook_attack(const Square sq) {
		return bit::g_rook_attack[bit::g_rook_offset[sq]];
	}
	inline bit::Bitboard get_silver_attack(const Side sd, const Square sq) {
		return bit::g_silver_attacks[sd][sq];
	}
	inline bit::Bitboard get_gold_attack(const Side sd, const Square sq) {
		return bit::g_gold_attacks[sd][sq];
	}
	inline bit::Bitboard get_pawn_attack(const Side sd, const Square sq) {
		//歩は数が多いから専用のテーブル用意したほうがいいかも
		return get_gold_attack(sd, sq) & get_silver_attack(sd, sq) & get_gold_attack(flip_turn(sd),sq);
	}
	template<Side sd> bit::Bitboard get_pawn_attack(const bit::Bitboard pawn) {
		return (sd == BLACK) ? pawn >> 1 : pawn << 1;
	}
	inline bit::Bitboard get_king_attack(const Square sq) {
		return get_gold_attack(BLACK, sq) | get_gold_attack(WHITE, sq);
	}
	inline bit::Bitboard get_prook_attack(const Square sq, const bit::Bitboard& occ) {
		return (get_rook_attack(sq, occ) | get_king_attack(sq));
	}
	inline bit::Bitboard get_pseudo_prook_attack(const Square sq) {
		return (get_pseudo_rook_attack(sq) | get_king_attack(sq));
	}
	inline bit::Bitboard get_pbishop_attack(const Square sq, const bit::Bitboard& occ) {
		return (get_bishop_attack(sq, occ) | get_king_attack(sq));
	}
	inline bit::Bitboard get_pseudo_pbishop_attack(const Square sq) {
		return (get_pseudo_bishop_attack(sq) | get_king_attack(sq));
	}
	inline bit::Bitboard get_plus_attack(const Square sq) {
		return get_gold_attack(BLACK, sq) & get_gold_attack(WHITE, sq);
	}
	inline bit::Bitboard get_x_attack(const Square sq) {
		return get_silver_attack(BLACK, sq) & get_silver_attack(WHITE, sq);
	}

	inline bool line_is_empty(const Square from, const Square to, const bit::Bitboard& bb) {
		return !(gBetween[gBetweenIndex[from][to]] & bb);
	}

	inline Bitboard between(const Square from, const Square to) {
		return gBetween[gBetweenIndex[from][to]];
	}

	inline Bitboard beyond(const Square from, const Square to) {
		return gBeyond[gBeyondIndex[from][to]];
	}
	template<Piece pc>Bitboard piece_pseudo_attacks(const Side sd, const Square from) {
		switch (pc) {
		case Pawn: return get_pawn_attack(sd, from);
		case Silver: return get_silver_attack(sd, from);
		case Gold:
		case Golds:
		case PPawn:
		case PSilver: return get_gold_attack(sd, from);
		case King: return get_king_attack(from);
		case Rook: return get_pseudo_rook_attack(from);
		case Bishop: return get_pseudo_bishop_attack(from);
		case PRook: return get_pseudo_prook_attack(from);
		case PBishop: return get_pseudo_pbishop_attack(from);
		default: return Bitboard(0ull);
		}
	}
	template<Piece pc>Bitboard piece_attacks(const Side sd, const Square from, const Bitboard& pieces) {
		switch (pc) {
		case Pawn: return get_pawn_attack(sd, from);
		case Silver: return get_silver_attack(sd, from);
		case Gold:
		case Golds:
		case PPawn:
		case PSilver: return get_gold_attack(sd, from);
		case King: return get_king_attack(from);
		case Rook: return get_rook_attack(from, pieces);
		case Bishop: return get_bishop_attack(from, pieces);
		case PRook: return get_prook_attack(from, pieces);
		case PBishop: return get_pbishop_attack(from, pieces);
		default: return Bitboard(0ull);
		}
	}
	Bitboard piece_attacks(const Piece pc, const Side sd, const Square from, const Bitboard &pieces);

	template<Piece pc>bool piece_pseudo_attack(const Side sd, const Square from, const Square to) {
		return piece_pseudo_attacks<pc>(sd, from).is_set(to);
	}
	template<Piece pc>bool piece_attack(const Side sd, const Square from, const Square to, const Bitboard& pieces) {
		return piece_pseudo_attack<pc>(sd, from, to) && line_is_empty(from, to, pieces);
	}
	inline bool piece_attack(const Piece pc, const Side sd, const Square from, const Square to, const Bitboard& pieces) {
		switch (pc) {
		case Pawn: return piece_attack<Pawn>(sd, from, to, pieces);
		case Silver: return piece_attack<Silver>(sd, from, to, pieces);
		case Gold:
		case Golds:
		case PPawn:
		case PSilver: return piece_attack<Gold>(sd, from, to, pieces);
		case King: return piece_attack<King>(sd, from, to, pieces);
		case Rook: return piece_attack<Rook>(sd, from, to, pieces);
		case Bishop: return piece_attack<Bishop>(sd, from, to, pieces);
		case PRook: return piece_attack<PRook>(sd, from, to, pieces);
		case PBishop: return  piece_attack<PBishop>(sd, from, to, pieces);
		default: return false;
		}
	}


	void init();
	void test();

}



#endif