#include "common.hpp"
#include "bit.hpp"

Direction gDirection[SQUARE_SIZE][SQUARE_SIZE];


Square sq_from_string(const std::string& s) {
	assert(s.length() == 2);
	return square_make(File(s[0] - '1'), Rank(s[1] - 'a'));
}
std::string sq_to_string(const Square sq) {
	std::string s;
	s += '1' + square_file(sq);
	s += 'a' + square_rank(sq);
	return s;
}
bool square_is_prom(const Side sd, const Square sq) {
	const auto rank = square_rank(sq);
	return (sd == BLACK) ? (rank == Rank_1) : (rank >= Rank_5);
}
namespace common {

	void init() {
	}

}