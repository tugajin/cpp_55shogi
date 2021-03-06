#include "pos.hpp"
#include "move.hpp"
#include "hand.hpp"
#include "sfen.hpp"
#include "bit.hpp"
#include "attack.hpp"

Pos::Pos(Side turn, Bitboard piece_side[], int hand[]) {
	this->clear();
	PIECE_SIDE_FOREACH(ps) {
		auto pc = piece_side_piece(ps);
		auto sd = piece_side_side(ps);
		for (auto bb = piece_side[ps]; bb; ) {
			auto sq = bb.lsb();
			this->add_piece(pc, sd, sq);
		}
	}
	this->hand_[BLACK] = this->hand_[WHITE] = HAND_NONE;

	SIDE_FOREACH(sd) {
		HAND_FOREACH(pc) {
			auto ps = piece_side_make(pc, sd);
			this->hand_[sd] = hand_change<true>(this->hand_[sd], pc, hand[ps]);
			for (auto num = 0; num <= hand[ps]; num++) {
				this->hand_key_ ^= hash::key_hand(pc, sd, num);
			}
		}
	}
	if (turn == WHITE) {
		this->switch_turn();
	}

	this->update();

	assert(is_ok());
}
void Pos::clear() {
	for (auto& piece : this->piece_) {
		piece.init();
	}
	for (auto& sd : this->side_) {
		sd.init();
	}
	this->all_.init();
	this->turn_ = BLACK;
	this->ply_ = Ply(0);
	for (auto& sq : this->square_) {
		sq = PieceNone;
	}
	this->last_move_ = move::MOVE_NONE;
	this->cap_sq_ = SQ_NONE;
	this->pos_key_ = Key(0ULL);
	this->hand_key_ = Key(0ULL);
	this->parent_ = nullptr;

}
Pos Pos::succ(const Move move) const {
#ifdef DEBUG
	if (!is_ok()) {
		Tee << *this << std::endl;
	}
	if (!move::move_is_ok(move, *this)) {
		Tee << *this << std::endl;
		Tee << move::move_to_string(move) << std::endl;
	}
	assert(is_ok());
	assert(move::move_is_ok(move, *this));
#endif
	const auto to = move::move_to(move);
	const auto sd = this->turn();
	const auto xd = flip_turn(sd);
	auto pos = *this;
	pos.parent_ = this;
	pos.ply_ = this->ply_ + 1;
	pos.last_move_ = move;
	const auto pc = move::move_piece(move);
	if (move::move_is_drop(move)) {
		auto num = hand_num(pos.hand_[sd], pc);
		pos.hand_[sd] = hand_change<false>(pos.hand_[sd], pc);
		pos.hand_key_ ^= hash::key_hand(pc, sd, num);
		pos.add_piece(pc, sd, to);
	}
	else {
		const auto from = move::move_from(move);
		const auto cap = move::move_cap(move);
		const auto prom = move::move_is_prom(move);
		if (cap != PieceNone) {
			pos.remove_piece(cap, xd, to);
			pos.cap_sq_ = to;
			const auto unprom_cap = piece_unprom(cap);
			auto num = hand_num(pos.hand_[sd], unprom_cap);
			pos.hand_[sd] = hand_change<true>(pos.hand_[sd], unprom_cap);
			pos.hand_key_ ^= hash::key_hand(unprom_cap, sd, num + 1);
		}
		pos.remove_piece(pc, sd, from);
		if (prom) {
			pos.add_piece(piece_prom(pc), sd, to);
		}
		else {
			pos.add_piece(pc, sd, to);
		}
	}
	pos.switch_turn();
	pos.update();
	assert(pos.is_ok());
	return pos;
}

bool Pos::is_ok() const {
	if (!hand_is_ok(this->hand_[BLACK])) {
		Tee << "black hand error";
		return false;
	}
	if (!hand_is_ok(this->hand_[WHITE])) {
		Tee << "white hand error";
		return false;
	}
	auto tmp_all = this->side_[BLACK] | this->side_[WHITE];
	if (tmp_all != this->all_) {
		Tee << "all error";
		Tee << tmp_all << std::endl;
		Tee << this->all_ << std::endl;
		return false;
	}
	PIECE_FOREACH(pc) {
		auto bb = this->piece_[pc];
		while (bb) {
			auto sq = bb.lsb();
			if (!this->all_.is_set(sq)) {
				Tee << "piece -> all error" << std::endl;
				Tee << pc << std::endl;
				Tee << sq << std::endl;
				Tee << this->piece_[pc] << std::endl;
				return false;
			}
			if (this->square_[sq] != pc) {
				Tee << "pc error" << std::endl;
				Tee << this->square_[sq] << std::endl;
				Tee << pc << std::endl;
				Tee << sq << std::endl;
				return false;
			}
		}
	}
	while (tmp_all) {
		const auto sq = tmp_all.lsb();
		if (this->square_[sq] == PieceNone) {
			Tee << "sq error" << std::endl;
			Tee << this->square_[sq] << std::endl;
			Tee << sq << std::endl;
			return false;
		}
	}
	if (this->ply_ < 0) {
		Tee << "ply error" << std::endl;
		Tee << this->ply_ << std::endl;
		return false;
	}
	auto debug_key = hash::key_turn(this->turn());
	SQUARE_FOREACH(sq) {
		const auto pc = this->square_[sq];
		if (pc != PieceNone) {
			const auto sd = this->side(sq);
			debug_key ^= hash::key_piece(pc, sd, sq);
		}
	}
	if (this->pos_key_ != debug_key) {
		Tee << "pos_key not eq" << std::endl;
		Tee << uint64(debug_key) << std::endl;
		Tee << uint64(this->pos_key_) << std::endl;
		return false;
	}
	debug_key = Key(0ULL);
	HAND_FOREACH(pc) {
		for (auto num = 0; num <= hand_num(this->hand_[BLACK], pc); num++) {
			debug_key ^= hash::key_hand(pc, BLACK, num);
		}
		for (auto num = 0; num <= hand_num(this->hand_[WHITE], pc); num++) {
			debug_key ^= hash::key_hand(pc, WHITE, num);
		}
	}
	if (debug_key != this->hand_key()) {
		Tee << "hand_key not eq" << std::endl;
		Tee << uint64(debug_key) << std::endl;
		Tee << uint64(this->hand_key_) << std::endl;
		return false;
	}
	return true;
}
std::ostream& operator<<(std::ostream& os, const Pos& b) {
	if (b.turn() == BLACK) {
		os << "BLACK" << std::endl;
	}
	else {
		os << "WHITE" << std::endl;
	}

	std::string piece_str[] = {" ・"," 歩"," 銀"," 角"," 飛"," 金"," 玉"," ・"," ・"," と"," 全"," 馬"," 龍"
	  ," ・"," ・"," ・", " ・","v歩","v銀","v角","v飛","v金","v玉","v・","v・","vと","v全","v馬","v龍",};
	

	os << "pos_key:" << uint64(b.pos_key()) << std::endl;
	os << "hand_key:" << uint64(b.hand_key()) << std::endl;
	os << "ply:" << b.ply() << std::endl;
	os << out_sfen(b) << std::endl;
	os << hand_to_string(b.hand(WHITE)) << std::endl;
	RANK_FOREACH(rank) {
		if (!rank) {
			os << "  ";
			FILE_FOREACH_REV(file) {
				os << "  " << file + 1;
			}
			os << "\n";
		}
		FILE_FOREACH_REV(file) {
			if (file == FILE_SIZE - 1) {
				os << char(int(rank) + 'a') << ":";
			}
			const auto sq = square_make(file, rank);
			const auto pc = b.piece(sq);
			if (pc == PieceNone) {
				os << piece_str[pc];
			}
			else {
				const auto sd = b.side(sq);
				const auto p32 = piece_side_make(pc, sd);
				os << piece_str[p32];
				//os << piece_side_to_sfen(p32);
			}
		}
		os << "\n";
	}
	os << hand_to_string(b.hand(BLACK)) << std::endl;
	return os;
}

Pos::RepState Pos::is_draw() const {
	if (this->ply() < 8) {
		return Pos::RepNone;
	}
	auto pos = this;
	for (auto i = 0; i < 4; i++) {
		assert(pos->parent_ != nullptr);
		auto org = pos;
		pos = pos->parent_->parent_;
		assert(org != nullptr);
		assert(pos != nullptr);

		if (pos->pos_key() == this->pos_key()) {
			if (pos->hand_key() == this->hand_key()) {
				if (in_check(*this) && in_check(*org)) {
					return Pos::RepChecked;
				}
				else if (in_check(*org->parent_)) {
					return Pos::RepCheck;
				}
				else {
					return Pos::RepEq;
				}
			}
			if (hand_is_superior(this->hand_[BLACK], pos->hand_[BLACK])) {
				if (this->turn() == BLACK) {
					return Pos::RepHandWin;
				}
				else {
					return Pos::RepHandLose;
				}
			}
			if (hand_is_superior(pos->hand_[BLACK], this->hand_[BLACK])) {
				if (this->turn() == BLACK) {
					return Pos::RepHandLose;
				}
				else {
					return Pos::RepHandWin;
				}
			}
		}
	}
	return Pos::RepNone;
}

bool Pos::is_win() const {
	const auto sd = this->turn();
	const auto king_sq = this->king(sd);
	const auto king_rank = square_rank(king_sq);
	if (sd == BLACK) {
		if (side_rank<BLACK>(king_rank) >= Rank_4) {
			return false;
		}
	}
	else {
		if (side_rank<WHITE>(king_rank) >= Rank_4) {
			return false;
		}
	}
	if (in_check(*this)) {
		return false;
	}
	auto side_bb = this->pieces(sd);
	side_bb.clear(king_sq);
	side_bb &= g_prom[sd];
	if (side_bb.pop_cnt() < 10) {
		return false;
	}
	auto num = this->pieces(Pawn, sd).pop_cnt();
	num += this->pieces(Silver, sd).pop_cnt();
	num += this->golds(sd).pop_cnt();
	num += (this->pieces(Bishop, sd).pop_cnt()) * 5;
	num += (this->pieces(Rook, sd).pop_cnt()) * 5;
	num += (this->pieces(PBishop, sd).pop_cnt()) * 5;
	num += (this->pieces(PRook, sd).pop_cnt()) * 5;

	num += hand_num(this->hand(sd), Pawn);
	num += hand_num(this->hand(sd), Silver);
	num += hand_num(this->hand(sd), Gold);
	num += hand_num(this->hand(sd), Bishop) * 5;
	num += hand_num(this->hand(sd), Rook) * 5;

	return (num >= 31);
}


namespace pos {

	Pos gStart;

	void init() {
		gStart = pos_from_sfen(START_SFEN);
	}

	static void test_gen() {
		Pos p = pos_from_sfen(START_SFEN);
		Tee << p << std::endl;

		Tee << "end test pos\n";
		bit::Bitboard bb;


	}

	void test_draw() {
		{
			auto p = pos_from_sfen("9/9/ppppppppp/4k4/1R7/3PP4/9/9/4K4 b - 1");

			auto mv1 = move::from_usi("8e5e", p);
			auto p1 = p.succ(mv1);
			Tee << p1 << std::endl;;

			auto mv2 = move::from_usi("5d6d", p1);
			auto p2 = p1.succ(mv2);
			Tee << p2 << std::endl;;

			auto mv3 = move::from_usi("5e6e", p2);
			auto p3 = p2.succ(mv3);
			Tee << p3 << std::endl;;

			auto mv4 = move::from_usi("6d5d", p3);
			auto p4 = p3.succ(mv4);
			Tee << p4 << std::endl;;

			auto mv5 = move::from_usi("6e5e", p4);
			auto p5 = p4.succ(mv5);
			Tee << p5 << std::endl;;
			Tee << p5.is_draw() << std::endl;
			assert(p5.is_draw() == Pos::RepChecked);
			auto mv6 = move::from_usi("5d6d", p5);
			auto p6 = p5.succ(mv6);
			Tee << p6 << std::endl;;

			Tee << p6.is_draw() << std::endl;
			assert(p6.is_draw() == Pos::RepCheck);
		}
		{
			auto p = pos_from_sfen("4k4/9/9/9/9/9/9/9/4K4 b - 1");

			auto mv1 = move::from_usi("5i5h", p);
			auto p1 = p.succ(mv1);
			Tee << p1 << std::endl;;

			auto mv2 = move::from_usi("5a5b", p1);
			auto p2 = p1.succ(mv2);
			Tee << p2 << std::endl;;

			auto mv3 = move::from_usi("5h5i", p2);
			auto p3 = p2.succ(mv3);
			Tee << p3 << std::endl;;

			auto mv4 = move::from_usi("5b5a", p3);
			auto p4 = p3.succ(mv4);
			Tee << p4 << std::endl;;
			Tee << p4.is_draw() << std::endl;;
			assert(p4.is_draw() == Pos::RepEq);

		}
		{
			auto p = pos_from_sfen("4k4/9/9/9/9/9/9/9/4K4 b 3G - 1");

			auto mv1 = move::from_usi("G*6a", p);
			auto p1 = p.succ(mv1);
			Tee << p1 << std::endl;;

			auto mv2 = move::from_usi("5a6a", p1);
			auto p2 = p1.succ(mv2);
			Tee << p2 << std::endl;;

			auto mv3 = move::from_usi("G*5a", p2);
			auto p3 = p2.succ(mv3);

			auto mv4 = move::from_usi("6a5a", p3);
			auto p4 = p3.succ(mv4);
			Tee << p4 << std::endl;;
			Tee << p4.is_draw() << std::endl;;
			assert(p4.is_draw() == Pos::RepHandLose);

			auto mv5 = move::from_usi("G*6a", p4);
			auto p5 = p4.succ(mv5);
			Tee << p5 << std::endl;
			assert(p5.is_draw() == Pos::RepHandWin);


		}
	}

	void test() {
		Tee << "start test pos\n";
		//test_gen();    
		test_draw();
	}

}