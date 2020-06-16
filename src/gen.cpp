#include "common.hpp"
#include "gen.hpp"
#include "bit.hpp"
#include "pos.hpp"
#include "list.hpp"
#include "attack.hpp"
#include "sfen.hpp"


template<Piece pc, Side sd>void add_noprom_move(List& ml, const Pos& pos, const bit::Bitboard& target) {
	auto piece = (pc == Golds) ? pos.golds(sd) : pos.pieces(pc, sd);
	while (piece) {
		const auto from = piece.lsb();
		for (auto att = piece_attacks<pc>(sd, from, pos.pieces()) & target; att;) {
			const auto to = att.lsb();
			ml.add(move::make_move(from, to, pos.piece(from), pos.piece(to)));
		}
	}
}
template<Side sd>void add_king_move(List& ml, const Pos& pos, const bit::Bitboard& target) {
	const auto from = pos.king(sd);
	for (auto att = get_king_attack(from) & target; att;) {
		const auto to = att.lsb();
		ml.add(move::make_move(from, to, King, pos.piece(to)));
	}
}
template<Side sd>void add_pawn_move(List& ml, const Pos& pos, const bit::Bitboard& target) {
	for (auto att = get_pawn_attack<sd>(pos.pieces(Pawn, sd)) & target; att;) {
		const auto to = att.lsb();
		const auto from = (sd == BLACK) ? to + SQ_RANK_INC : to - SQ_RANK_INC;
		ml.add(move::make_move(from, to, Pawn, pos.piece(to), square_is_prom<sd>(to)));
	}
}
template<Piece pc, Side sd>void add_bishop_rook_move(List& ml, const Pos& pos, const bit::Bitboard& target) {
	static_assert(pc == Bishop || pc == Rook, "pc error");
	for (auto piece = pos.pieces(pc, sd); piece;) {
		const auto from = piece.lsb();
		for (auto att = piece_attacks<pc>(sd, from, pos.pieces()) & target; att;) {
			const auto to = att.lsb();
			const auto prom = square_is_prom<sd>(from) || square_is_prom<sd>(to);
			ml.add(move::make_move(from, to, pc, pos.piece(to), prom));
		}
	}
}
template<Side sd>void add_silver_move(List& ml, const Pos& pos, const bit::Bitboard& target) {
	for (bit::Bitboard piece = pos.pieces(Silver, sd); piece;) {
		const auto from = piece.lsb();
		for (auto att = piece_attacks<Silver>(sd, from, pos.pieces()) & target; att;) {
			const auto to = att.lsb();
			const auto prom = square_is_prom<sd>(from) || square_is_prom<sd>(to);
			if (prom) {
				ml.add(move::make_move(from, to, Silver, pos.piece(to), true));
			}
			ml.add(move::make_move(from, to, Silver, pos.piece(to), false));
		}
	}
}
template<bool has_pawn, Side sd, int num>
void add_drop_move(List& ml, const Pos& pos, const bit::Bitboard& target, const Piece p[]) {
	if (has_pawn) {
		//pawn
		const auto mask = bit::g_rank_mask[Rank_1] | bit::g_rank_mask[Rank_2] | bit::g_rank_mask[Rank_3] | bit::g_rank_mask[Rank_4];
		const auto pawn = pos.pieces(Pawn,sd) + mask;
		const auto index = (uint32)(ml::pext(pawn.p(), bit::g_rank_mask[Rank_5].p()));
		auto pawn_target = target & bit::g_double_pawn_mask[sd][index];
		const auto mate_with_sq = (sd == BLACK) ?
			pos.king(flip_turn(sd)) + SQ_RANK_INC :
			pos.king(flip_turn(sd)) - SQ_RANK_INC;
		if (is_valid_sq(mate_with_sq)
			&& pawn_target.is_set(mate_with_sq)
			&& is_mate_with_pawn_drop(mate_with_sq, pos)) {
			pawn_target.clear(mate_with_sq);
		}
		while (pawn_target) {
			ml.add(move::make_move(pawn_target.lsb(), Pawn));
		}

	}

	//rank1
	bit::Bitboard rank_1 = target & bit::g_prom[sd];
	while (bool(rank_1)) {
		const auto to = rank_1.lsb();
		for (auto i = 0; i < num; i++) {
			ml.add(move::make_move(to, p[i]));
		}
	}

	//rank2~rank5
	bit::Bitboard rank_25 = target & bit::g_middle[sd];
	while (bool(rank_25)) {
		const auto to = rank_25.lsb();
		for (auto i = 0; i < num; i++) {
			ml.add(move::make_move(to, p[i]));
		}
	}
}
template<Side sd>
void add_drop_move(List& ml, const Pos& pos, const bit::Bitboard& target) {

	const auto hand = pos.hand(sd);
	auto status = 0;
	Piece piece_list[4] = { };
	Piece* p = piece_list;

	(hand_has(hand, Pawn)) ? status += 1 : status;
	(hand_has(hand, Silver)) ?
		status += 2, (*p++) = Silver : status;
	(hand_has(hand, Gold)) ? status += 2, (*p++) = Gold : status;
	(hand_has(hand, Bishop)) ?
		status += 2, (*p++) = Bishop : status;
	(hand_has(hand, Rook)) ? status += 2, (*p++) = Rook : status;

#define ADD_DROP_MOVE(n) {\
  constexpr auto pawn_flag   = (n & (1 << 0)) != 0;\
  constexpr auto num = n >> 1;\
  add_drop_move<pawn_flag,sd,num>(ml,pos,target,piece_list); \
  break;\
}
	switch (status) {
	case 0:
		break;
	case 1:
		ADD_DROP_MOVE(1)
	case 2:
		ADD_DROP_MOVE(2)
	case 3:
		ADD_DROP_MOVE(3)
	case 4:
		ADD_DROP_MOVE(4)
	case 5:
		ADD_DROP_MOVE(5)
	case 6:
		ADD_DROP_MOVE(6)
	case 7:
		ADD_DROP_MOVE(7)
	default:
		assert(false);
	}
#undef ADD_DROP_MOVE
}

template<Side sd>void gen_legals(List& list, const Pos& pos) {
	List tmp;
	gen_moves<sd>(tmp, pos);
	list.clear();
	for (auto i = 0; i < tmp.size(); i++) {
		const auto mv = tmp[i];
		if (move::pseudo_is_legal(mv, pos)) {
			list.add(mv);
		}
	}
}
void gen_legals(List& list, const Pos& pos) {
	(pos.turn() == BLACK) ? gen_legals<BLACK>(list, pos)
		: gen_legals<WHITE>(list, pos);
}
template<MoveType mt, Side sd>
void gen_moves(List& ml, const Pos& pos, const bit::Bitboard* checks) {

	constexpr auto xd = flip_turn(sd);
	bit::Bitboard target, target2;
	switch (mt) {
	case TACTICAL:
		target = pos.pieces(xd);
		target2 = target | (~pos.pieces(sd) & bit::g_prom[sd]);
		break;
	case QUIET:
		target = ~pos.pieces();
		target2 = target & bit::g_middle[sd];
		break;
	case DROP:
		target = ~pos.pieces();
		break;
	case EVASION: {
		//取れるなら取ってしまったほうがいい気がする
		target = ~pos.pieces(sd);
		add_king_move<sd>(ml, pos, target);
		assert(checks != nullptr);
		if (checks->pop_cnt() == 2) {
			return;
		}
#ifdef DEBUG
		if(checks->pop_cnt() != 1) {
			Tee<<"error evasion\n";
			Tee<<pos<<std::endl;
			Tee<<move::move_to_string(pos.last_move())<<std::endl;
			Tee<<*checks<<std::endl;
		}
#endif
		assert(checks->pop_cnt() == 1);
		auto check_bb = *checks;
		const auto checker_sq = check_bb.lsb<false>();
		target = target2 = between(checker_sq, pos.king(sd));
		target.set(checker_sq);//capture checker
		add_pawn_move<sd>(ml, pos, target);
		add_silver_move<sd>(ml, pos, target);
		add_noprom_move<Golds, sd>(ml, pos, target);
		add_bishop_rook_move<Bishop, sd>(ml, pos, target);
		add_bishop_rook_move<Rook, sd>(ml, pos, target);
		add_noprom_move<PBishop, sd>(ml, pos, target);
		add_noprom_move<PRook, sd>(ml, pos, target);

		add_drop_move<sd>(ml, pos, target2);
		return;
	}
	default:
		assert(false);
		break;
	}
	switch (mt) {
	case DROP:
		add_drop_move<sd>(ml, pos, target);
		break;
	default:
		add_pawn_move<sd>(ml, pos, target2);
		add_silver_move<sd>(ml, pos, target);
		add_noprom_move<Golds, sd>(ml, pos, target);
		add_king_move<sd>(ml, pos, target);
		add_bishop_rook_move<Bishop, sd>(ml, pos, target);
		add_bishop_rook_move<Rook, sd>(ml, pos, target);
		add_noprom_move<PBishop, sd>(ml, pos, target);
		add_noprom_move<PRook, sd>(ml, pos, target);
		break;
	}
}
template<Side sd>void gen_moves(List& list, const Pos& pos) {
	const auto chk = checks(pos);
	gen_moves<sd>(list, pos, &chk);
}
template<Side sd>void gen_moves(List& list, const Pos& pos, const bit::Bitboard* checks) {
	if (!(*checks)) {
		gen_moves<TACTICAL, sd>(list, pos, checks);
		gen_moves<QUIET, sd>(list, pos, checks);
		gen_moves<DROP, sd>(list, pos, checks);
	}
	else {
		gen_moves<EVASION, sd>(list, pos, checks);
	}
}
void gen_moves(List& list, const Pos& pos) {
	(pos.turn() == BLACK) ? gen_moves<BLACK>(list, pos) : gen_moves<WHITE>(list, pos);
}

template void gen_moves<BLACK>(List& ml, const Pos& pos);
template void gen_moves<WHITE>(List& ml, const Pos& pos);

template void gen_moves<BLACK>(List& ml, const Pos& pos, const bit::Bitboard* checks);
template void gen_moves<WHITE>(List& ml, const Pos& pos, const bit::Bitboard* checks);

template void gen_moves<TACTICAL, BLACK>(List& ml, const Pos& pos, const bit::Bitboard* checks);
template void gen_moves<TACTICAL, WHITE>(List& ml, const Pos& pos, const bit::Bitboard* checks);

template void gen_moves<QUIET, BLACK>(List& ml, const Pos& pos, const bit::Bitboard* checks);
template void gen_moves<QUIET, WHITE>(List& ml, const Pos& pos, const bit::Bitboard* checks);

template void gen_moves<DROP, BLACK>(List& ml, const Pos& pos, const bit::Bitboard* checks);
template void gen_moves<DROP, WHITE>(List& ml, const Pos& pos, const bit::Bitboard* checks);

template void gen_moves<EVASION, BLACK>(List& ml, const Pos& pos, const bit::Bitboard* checks);
template void gen_moves<EVASION, WHITE>(List& ml, const Pos& pos, const bit::Bitboard* checks);


namespace gen {

	void test() {
		{
			Pos pos = pos_from_sfen(START_SFEN);
			Tee<<pos<<std::endl;
			List list;
			gen_legals(list,pos);
			Tee<<list<<std::endl;
		}
		{
			Pos pos = pos_from_sfen("4k/5/5/5/3PK b P - 1");
			Tee<<pos<<std::endl;
			List list;
			gen_legals(list,pos);
			Tee<<list<<std::endl;
		}
		{
			Pos pos = pos_from_sfen("4k/5/5/5/2P1K b P - 1");
			Tee<<pos<<std::endl;
			List list;
			gen_legals(list,pos);
			Tee<<list<<std::endl;
		}
		{
			Pos pos = pos_from_sfen("4k/5/5/5/1P2K b P - 1");
			Tee<<pos<<std::endl;
			List list;
			gen_legals(list,pos);
			Tee<<list<<std::endl;
		}
		{
			Pos pos = pos_from_sfen("4k/5/5/5/P3K b P - 1");
			Tee<<pos<<std::endl;
			List list;
			gen_legals(list,pos);
			Tee<<list<<std::endl;
		}
	}

}

