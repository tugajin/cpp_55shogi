#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "common.hpp"
#include "util.hpp"
#include "movelist.hpp"
namespace game {
class Position;
}
namespace hash {
Key hash_key(const game::Position &pos);
}
namespace sfen{
std::string out_sfen(const game::Position &pos);
}

bool move_is_ok(const Move m, const game::Position &pos); 

namespace game {

class Neighbor {
/*
    0
   1 7
  2   6
   3 5
    4 
*/
private:
    Square dir[SQ_END][DIR_SIZE];
public:
    Neighbor() {
        REP(sq,SQ_END) {
#define INIT_NEI(d, index) do {\
            Square sq2 = static_cast<Square>(sq);\
            do { sq2 += d; }while (sq_is_ok(sq2));\
            this->dir[sq][index] = sq2;\
} while(0)
            INIT_NEI(INC_UP,0);
            INIT_NEI(INC_DOWN,4);
            INIT_NEI(INC_LEFTUP,1);
            INIT_NEI(INC_RIGHTDOWN,5);
            INIT_NEI(INC_LEFT,2);
            INIT_NEI(INC_RIGHT,6);
            INIT_NEI(INC_LEFTDOWN,3);
            INIT_NEI(INC_RIGHTUP,7);
#undef INIT_NEI
        }
    }
    void dump() const {
        REP(sq, SQ_END) {
            Tee<<"sq is"<<sq<<" "
               <<this->dir[sq][0] << ":"
               <<this->dir[sq][1] << ":"
               <<this->dir[sq][2] << ":"
               <<this->dir[sq][3] << ":"
               <<this->dir[sq][4] << ":"
               <<this->dir[sq][5] << ":"
               <<this->dir[sq][6] << ":"
               <<this->dir[sq][7] << "\n";
        }
    }
    Square neighbor(const Square sq, const int dir) const {
        ASSERT(sq>=0);
        ASSERT(sq<SQ_END);
        ASSERT(dir>=0);
        ASSERT(dir<8);
        return this->dir[sq][dir];
    }
    void evacuate(const Square sq) {
        REP(d, 4) {
            const auto up = this->neighbor(sq,d);
            const auto dn = this->neighbor(sq,d+4);
            ASSERT(up>=0);
            ASSERT(up<SQ_END);
            ASSERT(dn>=0);
            ASSERT(dn<SQ_END);
            this->dir[up][d+4] = dn;
            this->dir[dn][d] = up;
        }
    }
    void occupy(const ColorPiece square[], const Square sq) {
        ASSERT(sq_is_ok(sq));
        const Square vec[] = { INC_UP, INC_LEFTUP, INC_LEFT, INC_LEFTDOWN };
        //Tee<<"occupy:"<<sq<<std::endl;
        REP(d, 4) {
            const auto v = vec[d];
            auto up = sq;
            //Tee<<"check dir:"<<v<<std::endl;
            while(square[up+=v] == COLOR_EMPTY){
                ASSERT2(sq_is_ok(up),{
                    Tee<<up<<std::endl;
                });
            }
            //Tee<<"up:"<<up<<std::endl;
            const auto dn = this->neighbor(up,d+4);
            //Tee<<"dn:"<<dn<<std::endl;
            ASSERT(up>=0);
            ASSERT(up<SQ_END);
            ASSERT(dn>=0);
            ASSERT(dn<SQ_END);
            //Tee<<"up d+4 :" << sq<<std::endl;
            //Tee<<"dn d:" << sq<<std::endl;
            //Tee<<"sq d+4 :" << dn<<std::endl;
            //Tee<<"sq d :" << up<<std::endl;
            this->dir[up][d+4] = sq;
            this->dir[dn][d] = sq;
            this->dir[sq][d+4] = dn;
            this->dir[sq][d] = up;
        }
        ASSERT2(this->is_ok(square),{
            REP(i, SQ_END) {
                Tee<<i<<":"<<square[i]<<std::endl;
            }
            this->dump();
        });
    }
    bool is_ok(const ColorPiece square[]) const {
        auto f = [&](const Square sq, const Square d, const Square neighbor) {
            Square sq2 = sq;
            do { sq2 += d; } while(square[sq2] == COLOR_EMPTY);
            if (neighbor == sq2) {
                return true;
            }
            Tee<<"not eq error neighbor\n";
            Tee<<"sq:"<<sq<<std::endl;
            Tee<<"inc:"<<d<<std::endl;
            Tee<<"nei:"<<neighbor<<std::endl;
            Tee<<"debug_nei:"<<sq2<<std::endl;
            return false;
        };
        for(auto *p = SQUARE_INDEX; *p != SQ_WALL; ++p) {
            const auto sq = *p;
            if (square[sq] == COLOR_EMPTY) {
                continue;
            }
            if (!f(sq, INC_UP, this->dir[sq][0])
            ||  !f(sq, INC_LEFTUP, this->dir[sq][1])
            ||  !f(sq, INC_LEFT, this->dir[sq][2])
            ||  !f(sq, INC_LEFTDOWN, this->dir[sq][3])
            ||  !f(sq, INC_DOWN, this->dir[sq][4])
            ||  !f(sq, INC_RIGHTDOWN, this->dir[sq][5])
            ||  !f(sq, INC_RIGHT, this->dir[sq][6])
            ||  !f(sq, INC_RIGHTUP, this->dir[sq][7])
            ) {
                Tee<<"nei error\n";
                Tee<<sq<<std::endl;
                return false;    
            }
            REP(i, 8) {
                if (this->dir[sq][i] == sq) {
                    Tee<<"nei error2\n";
                    Tee<<sq<<std::endl;
                    Tee<<i<<std::endl;
                    return false;
                }
                const auto inc = dir_to_inc(i);
                if (inc > 0) {
                    if (sq >= this->dir[sq][i]) {
                        Tee<<"nei error3\n";
                        Tee<<sq<<std::endl;
                        Tee<<this->dir[sq][i]<<std::endl;
                        Tee<<i<<std::endl;
                        return false;
                    }
                } else if (inc < 0) {
                    if (sq <= this->dir[sq][i]) {
                        Tee<<"nei error4\n";
                        Tee<<sq<<std::endl;
                        Tee<<this->dir[sq][i]<<std::endl;
                        Tee<<i<<std::endl;
                        return false;
                    }
                }
            }
        }
        return true;
    }
};

class Position {
public:
    Position() {}
    Position(const ColorPiece pieces[], const Hand hand[], const Color turn);
    void init_pos();
    bool is_lose() const ;
    bool is_win() const;
    bool is_draw(int max_num = 4) const {
        if (this->ply_ >= 400) {
            return true;
        }
        const auto curr_key = this->history_[this->ply_];
        auto num = 0;
        for (auto ply = this->ply_ - 4; ply >= 0; ply -= 2) {
            const auto prev_key = this->history_[ply];
            if (curr_key == prev_key) {
                if ((++num) >= max_num) {
                    return true;
                }
            }
        }
        return false;
    }
    bool is_draw_short() const {
        if (this->ply_ >= 400) {
            return true;
        }
        return false;
    }
    bool is_done() const {
        return (this->is_lose() || this->is_draw());
    }
    Color turn() const {
        return this->turn_;
    }
    Square piece_list(const Color c, const Piece p, const int index) const {
        ASSERT(color_is_ok(c));
        ASSERT(piece_is_ok(p));
        ASSERT(index >= 0 || index < 2);
        return this->piece_list_[c][p][index];
    }
    int piece_list_size(const Color c, const Piece p) const {
        ASSERT(color_is_ok(c));
        ASSERT(piece_is_ok(p));
        return this->piece_list_size_[c][p];
    }
    ColorPiece square(const Square sq) const {
        ASSERT(sq >= SQ_BEGIN);
        ASSERT(sq < SQ_END);
        return this->square_[sq];
    }
    Hand hand(const Color c) const {
        ASSERT(color_is_ok(c));
        return this->hand_[c];
    }
    bool is_ok() const;
    std::string str() const;
    Square king_sq (const Color c) const {
        return this->piece_list_[c][KING][0];
    }
	friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
        os << pos.str();
		return os;
	}
    Position next(const Move action) const;
    void dump() const;
    Key history() const {
        return this->history_[this->ply_];
    }
    Key history(const int ply) const {
        return this->history_[ply];
    }
    int ply() const {
        return this->ply_;
    }
    Square neighbor(const Square sq, const int dir) const {
        return this->neighbor_.neighbor(sq, dir);
    }
    bool exists_pawn(const Color c, const File f) const {
        return this->exist_pawn_[c][f];
    }
    Position mirror() const;
    Position rotate() const;
private:
    ColorPiece square_[SQ_END];//どんな駒が存在しているか？
    int piece_list_index_[SQ_END];//piece_listの何番目か

    Square piece_list_[COLOR_SIZE][PIECE_END][2];//駒がどのsqに存在しているか?
    int piece_list_size_[COLOR_SIZE][PIECE_END];//piece_listのどこまで使ったか？
    bool exist_pawn_[COLOR_SIZE][FILE_SIZE];//2歩用の配列
    Hand hand_[COLOR_SIZE];
    Key history_[1024];
    Neighbor neighbor_;
    
    int ply_;
    Color turn_;
    void quiet_move_piece(const Square from, const Square to, ColorPiece color_piece);
    void put_piece(const Square sq, const ColorPiece color_piece);
    void remove_piece(const Square sq, const ColorPiece color_piece);
};

Position::Position(const ColorPiece pieces[], const Hand hand[], const Color turn) {
   REP(i, SQ_END) {
        this->square_[i] = sq_is_ok(static_cast<Square>(i)) ? COLOR_EMPTY : COLOR_WALL;
        this->piece_list_index_[i] = -1;
    }
    REP_COLOR(col) {
        REP_PIECE(pc) {
            this->piece_list_[col][pc][0] = this->piece_list_[col][pc][1] = SQ_WALL;
            this->piece_list_size_[col][pc] = 0;
        }
        REP_FILE(file) {
            this->exist_pawn_[col][file] = false;
        }
    }
    REP(i, 1024) {
        this->history_[i] = 0;
    }
    this->ply_ = 0;
    REP(i, SQUARE_SIZE) {
        if (pieces[i] != COLOR_EMPTY) {
            this->put_piece(SQUARE_INDEX[i], pieces[i]);
            this->neighbor_.occupy(this->square_, SQUARE_INDEX[i]);
        } else {
            this->square_[SQUARE_INDEX[i]] = COLOR_EMPTY;
        }
    }
    this->hand_[BLACK] = hand[BLACK];
    this->hand_[WHITE] = hand[WHITE];
    this->turn_ = turn;
    this->history_[this->ply_] = hash::hash_key(*this);
    ASSERT2(this->is_ok(),{
        Tee<<*this<<std::endl;
        this->dump();
    })
}

void Position::quiet_move_piece(const Square from, const Square to, const ColorPiece color_piece) {
    ASSERT2(sq_is_ok(from),{ Tee<<"sq1:"<<from<<std::endl; });
    ASSERT2(sq_is_ok(to),{ Tee<<"sq2:"<<to<<std::endl; });
    ASSERT(color_piece != COLOR_EMPTY);
    const auto col = piece_color(color_piece);
    const auto pc = to_piece(color_piece);
    this->square_[from] = COLOR_EMPTY;
    this->square_[to] = color_piece;
    this->piece_list_index_[to] = this->piece_list_index_[from];
    this->piece_list_[col][pc][this->piece_list_index_[from]] = to;
    this->piece_list_index_[from] = -1;
    if (pc == PAWN) {
        this->exist_pawn_[col][sq_file(from)] = false;
        this->exist_pawn_[col][sq_file(to)] = true;
    }
}

void Position::put_piece(const Square sq, const ColorPiece color_piece) {
    ASSERT2(sq_is_ok(sq),{ Tee<<"sq:"<<sq<<std::endl; });
    ASSERT(color_piece != COLOR_EMPTY);
    const auto col = piece_color(color_piece);
    const auto pc = to_piece(color_piece);
    this->square_[sq] = color_piece;
    this->piece_list_index_[sq] = this->piece_list_size_[col][pc];
    this->piece_list_[col][pc][this->piece_list_size_[col][pc]] = sq;
    ++this->piece_list_size_[col][pc];
    if (pc == PAWN) {
        this->exist_pawn_[col][sq_file(sq)] = true;
    }
}

void Position::remove_piece(const Square sq, const ColorPiece color_piece) {
    const auto col = piece_color(color_piece);
    const auto pc = to_piece(color_piece);
    --this->piece_list_size_[col][pc];
    
    const auto last_sq = this->piece_list_[col][pc][this->piece_list_size_[col][pc]];
    const auto removed_index = this->piece_list_index_[sq];
    this->piece_list_[col][pc][removed_index] = last_sq;
    this->piece_list_index_[last_sq] = removed_index;

    this->square_[sq] = COLOR_EMPTY;
    this->piece_list_index_[sq] = -1;
    if (pc == PAWN) {
        this->exist_pawn_[col][sq_file(sq)] = false;
    }
}

std::string Position::str() const {
    std::string ret = "手番:" + color_str(this->turn()) + " hash:" + to_string(this->history_[this->ply_]) + " ply:" + to_string(this->ply_) + "\n";
    ret += sfen::out_sfen(*this)+"\n";
    ret += "後手:" + hand_str(this->hand_[WHITE]) + "\n";
    for(auto *p = SQUARE_INDEX; *p != SQ_WALL; ++p) {
        const auto color_piece = this->square_[*p];
        const auto col = piece_color(color_piece);
        const auto piece = to_piece(color_piece);
        if (piece == EMPTY) {
            ret += " ";
        } else if (col == BLACK) {
            ret += "^";
        } else {
            ret += "v";
        }
        ret += piece_str(piece);
        if (sq_file(*p) == FILE_1) { ret += "\n"; }
    }
    ret += "先手:" + hand_str(this->hand_[BLACK]) ;
    return ret;
}

bool Position::is_ok() const {
    if (this->turn() != BLACK && this->turn() != WHITE) {
#if DEBUG
                Tee<<"error turn\n";
#endif
        return false;
    }
    if (this->ply_ < 0 || this->ply_ > 1024) {
#if DEBUG
                Tee<<"error history\n";
#endif
        return false;
    }
    // square -> piece_list
    for(auto *p = SQUARE_INDEX; *p != SQ_WALL; ++p) {
        const auto sq = *p;
        const auto color_piece =this->square_[sq];
        if (color_piece != COLOR_EMPTY) {
            const auto piece = to_piece(color_piece);
            const auto color = piece_color(color_piece);
            const auto index = this->piece_list_index_[sq];
            if (index < 0 || index > 1) {
#if DEBUG
                Tee<<"error1\n";
#endif
                return false;
            }
            if (this->piece_list_[color][piece][index] == SQ_WALL) {
#if DEBUG
                Tee<<"error1.5\n";
                Tee<<this->piece_list_[color][piece][index]<<std::endl;
                Tee<<"color:"<<color<<std::endl;
                Tee<<"piece:"<<piece<<std::endl;
                Tee<<"index:"<<index<<std::endl;
                Tee<<"sq:"<<sq<<std::endl;
                return false;
#endif              
            }
            if (this->piece_list_[color][piece][index] != sq) {
#if DEBUG
                Tee<<"error2\n";
                Tee<<"sq:"<<sq<<std::endl;
                Tee<<this->piece_list_[color][piece][index]<<std::endl;
                Tee<<index<<std::endl;
                Tee<<color<<std::endl;
                Tee<<piece<<std::endl;
#endif
                return false;
            }
        } else {
            const auto index = this->piece_list_index_[sq];
            if (index != -1) {
#if DEBUG
                Tee<<"error3\n";
                Tee<<sq<<std::endl;
                Tee<<index<<std::endl;
#endif
                return false;
            }
        }
    }
    // piece_list -> square
    REP_COLOR(col) {
        REP_PIECE(piece) {
            if (this->piece_list_size_[col][piece] != 0
            && this->piece_list_size_[col][piece] != 1
            && this->piece_list_size_[col][piece] != 2) {
#if DEBUG
                Tee<<"error size\n";
                Tee<<this->piece_list_size_[col][piece]<<std::endl;
                Tee<<col<<std::endl;
                Tee<<piece<<std::endl;
#endif              
                return false;
            }
            REP(index, this->piece_list_size_[col][piece]) {
                const auto piece_list_sq = this->piece_list_[col][piece][index];
                if (color_piece(piece,col) != this->square_[piece_list_sq]) {
#if DEBUG
                Tee<<"error4\n";
#endif
                    return false;
                }
                if (this->piece_list_index_[piece_list_sq] != index) {
#if DEBUG
                Tee<<"error5\n";
#endif
                    return false;
                }
            }
        }
    }
    REP_FILE(file) {
        bool found_pawn[COLOR_SIZE] = {false, false};
        REP_RANK(rank) {
            const auto sq = ::square(file, rank);
            const auto color_piece = this->square(sq);
            const auto piece = to_piece(color_piece);
            const auto color = piece_color(color_piece);
            if (piece == PAWN) {
                found_pawn[color] = true;
            }
        }
        if (found_pawn[BLACK] != this->exists_pawn(BLACK, file)) {
#if DEBUG
                Tee<<"exists pawn error"<<file<<"\n";
#endif
        }
        if (found_pawn[WHITE] != this->exists_pawn(WHITE, file)) {
#if DEBUG
                Tee<<"exists pawn error2"<<file<<"\n";
#endif
        }
    }
    if (!hand_is_ok(this->hand_[BLACK])) {
#if DEBUG
        Tee<<"error black hand\n";
#endif
        return false;
    } 
    if (!hand_is_ok(this->hand_[WHITE])) {
#if DEBUG
        Tee<<"error white hand\n";
#endif
        return false;
    } 
    if (!this->neighbor_.is_ok(this->square_)) {
#if DEBUG
        Tee<<"error neighbor\n";
#endif
        return false;
    }

    return true;
}

Position Position::next(const Move action) const {
    ASSERT2(this->is_ok(),{
        Tee<<"prev_next\n";
        Tee<<this->str()<<std::endl;
        Tee<<move_str(action)<<std::endl;
    });
    ASSERT2(move_is_ok(action, *this), {
        Tee<< "prev move error\n";
        Tee<<this->str()<<std::endl;
        Tee<<move_str(action)<<std::endl;
    });
    Position next_pos = *this;
    const auto turn = next_pos.turn();
    if (move_is_drop(action)) {
        const auto piece = move_piece(action);
        const auto to = move_to(action);
        next_pos.put_piece(to, color_piece(piece,turn));
        next_pos.hand_[turn] = dec_hand(next_pos.hand_[turn], piece);
        next_pos.neighbor_.occupy(next_pos.square_, to);
    } else {
        const auto from = move_from(action);
        const auto to = move_to(action);
        const auto src_color_piece = next_pos.square(from);
        auto dst_color_piece = src_color_piece;
        const auto captured_color_piece = next_pos.square(to);
        next_pos.neighbor_.evacuate(from);
        if (captured_color_piece != COLOR_EMPTY) {
            next_pos.remove_piece(to, captured_color_piece);
            next_pos.hand_[turn] = inc_hand(next_pos.hand_[turn], unprom(to_piece(captured_color_piece)));
        } 
        if (move_is_prom(action)) {
            const auto src_piece = to_piece(src_color_piece);
            const auto dst_piece = prom(src_piece);
            dst_color_piece = color_piece(dst_piece, turn);
            next_pos.remove_piece(from, src_color_piece);
            next_pos.put_piece(to, dst_color_piece);
        } else {
            next_pos.quiet_move_piece(from, to, src_color_piece);
        }
        if(captured_color_piece == COLOR_EMPTY) {
            next_pos.neighbor_.occupy(next_pos.square_, to);
        }
    }
    ++next_pos.ply_;
    next_pos.turn_ = change_turn(turn);
    next_pos.history_[next_pos.ply_] = hash::hash_key(next_pos);
    ASSERT2(next_pos.is_ok(),{
        //Tee<<"prev\n";
        //Tee<<*this<<std::endl;
        Tee<<"after next\n";
        Tee<<next_pos<<std::endl;
        Tee<<move_str(action)<<std::endl;
        next_pos.dump();
    });
    return next_pos;
}

void Position::dump() const {
    Tee<<"dump:\n";
    Tee<<"square\n";
    REP(sq, SQ_END) {
        if (sq % 8 == 0) { Tee<<"\n";}
        Tee<<padding_str(to_string(static_cast<int>(this->square_[sq])),5)+",";
    }
    Tee<<"\nsquare_index\n";
    REP(sq, SQ_END) {
        if (sq % 8 == 0) { Tee<<"\n";}
        Tee<<padding_str(to_string(static_cast<int>(this->piece_list_index_[sq])),2)+",";
    }
    Tee<<"\npiece_size\n";
    REP_COLOR(col) {
        REP_PIECE(pc) {
            Tee << padding_str(to_string(this->piece_list_size_[col][pc]),2)+",";
        }
        Tee<<"\n";
    }
    Tee<<"piece_list\n";
    REP_COLOR(col) {
        REP_PIECE(pc) {
            Tee<<piece_str(Piece(pc))<<":";
            REP(index, 2) {
                Tee<<to_string(this->piece_list_[col][pc][index]) + ",";
            }
            Tee<<"\n";
        }
    }
    Tee<<"black_hand:"<<static_cast<int>(this->hand_[BLACK])<<std::endl;
    Tee<<"white_hand:"<<static_cast<int>(this->hand_[WHITE])<<std::endl;
    Tee<<"neighbor\n";
    this->neighbor_.dump();
}

Position Position::mirror() const {
    Position pos;
    pos.turn_ = this->turn_;
    pos.hand_[BLACK] = this->hand_[BLACK];
    pos.hand_[WHITE] = this->hand_[WHITE];
    for(auto *fp = SQUARE_INDEX; *fp != SQ_WALL; ++fp) {
        const auto msq = mirror_sq(*fp);
        pos.square_[msq] = this->square_[*fp];
    }
    pos.ply_ = 0;
    pos.history_[0] = hash::hash_key(pos);
    //FIXME 2歩のところがだめ
    ASSERT(pos.is_ok());
    return pos;
}

Position Position::rotate() const {
    Position pos;
    pos.turn_ = change_turn(this->turn_);
    pos.hand_[BLACK] = this->hand_[WHITE];
    pos.hand_[WHITE] = this->hand_[BLACK];

    for(auto *fp = SQUARE_INDEX; *fp != SQ_WALL; ++fp) {
        const auto isq = invert_sq(*fp);
        auto cp = this->square_[*fp];
        if (cp != COLOR_EMPTY) {
            const auto col = change_turn(piece_color(cp));
            const auto p = to_piece(cp);
            cp = color_piece(p, col);
        }
        pos.square_[isq] = cp;
    }

    pos.ply_ = 0;
    pos.history_[0] = hash::hash_key(pos);  
    ASSERT(pos.is_ok());
    return pos;
}

void test_common() {
#if 1
    ASSERT(change_turn(BLACK) == WHITE);
    ASSERT(change_turn(WHITE) == BLACK);

    ASSERT(square(FILE_1, RANK_1) == SQ_11);
    ASSERT(square(FILE_1, RANK_2) == SQ_12);
    ASSERT(square(FILE_1, RANK_3) == SQ_13);
    ASSERT(square(FILE_1, RANK_4) == SQ_14);
    ASSERT(square(FILE_1, RANK_5) == SQ_15);
    
    ASSERT(square(FILE_2, RANK_1) == SQ_21);
    ASSERT(square(FILE_2, RANK_2) == SQ_22);
    ASSERT(square(FILE_2, RANK_3) == SQ_23);
    ASSERT(square(FILE_2, RANK_4) == SQ_24);
    ASSERT(square(FILE_2, RANK_5) == SQ_25);

    ASSERT(square(FILE_3, RANK_1) == SQ_31);
    ASSERT(square(FILE_3, RANK_2) == SQ_32);
    ASSERT(square(FILE_3, RANK_3) == SQ_33);
    ASSERT(square(FILE_3, RANK_4) == SQ_34);
    ASSERT(square(FILE_3, RANK_5) == SQ_35);

    ASSERT(square(FILE_4, RANK_1) == SQ_41);
    ASSERT(square(FILE_4, RANK_2) == SQ_42);
    ASSERT(square(FILE_4, RANK_3) == SQ_43);
    ASSERT(square(FILE_4, RANK_4) == SQ_44);
    ASSERT(square(FILE_4, RANK_5) == SQ_45);

    ASSERT(square(FILE_5, RANK_1) == SQ_51);
    ASSERT(square(FILE_5, RANK_2) == SQ_52);
    ASSERT(square(FILE_5, RANK_3) == SQ_53);
    ASSERT(square(FILE_5, RANK_4) == SQ_54);
    ASSERT(square(FILE_5, RANK_5) == SQ_55);

    ASSERT(sq_file(SQ_11) == FILE_1);    
    ASSERT(sq_file(SQ_12) == FILE_1);    
    ASSERT(sq_file(SQ_13) == FILE_1);    
    ASSERT(sq_file(SQ_14) == FILE_1);
    ASSERT(sq_file(SQ_15) == FILE_1);

    ASSERT(sq_file(SQ_21) == FILE_2);    
    ASSERT(sq_file(SQ_22) == FILE_2);    
    ASSERT(sq_file(SQ_23) == FILE_2);    
    ASSERT(sq_file(SQ_24) == FILE_2);    
    ASSERT(sq_file(SQ_25) == FILE_2);    

    ASSERT(sq_file(SQ_31) == FILE_3);    
    ASSERT(sq_file(SQ_32) == FILE_3);    
    ASSERT(sq_file(SQ_33) == FILE_3);    
    ASSERT(sq_file(SQ_34) == FILE_3);
    ASSERT(sq_file(SQ_35) == FILE_3);

    ASSERT(sq_file(SQ_41) == FILE_4);    
    ASSERT(sq_file(SQ_42) == FILE_4);    
    ASSERT(sq_file(SQ_43) == FILE_4);    
    ASSERT(sq_file(SQ_44) == FILE_4);
    ASSERT(sq_file(SQ_45) == FILE_4);

    ASSERT(sq_file(SQ_51) == FILE_5);    
    ASSERT(sq_file(SQ_52) == FILE_5);    
    ASSERT(sq_file(SQ_53) == FILE_5);    
    ASSERT(sq_file(SQ_54) == FILE_5);
    ASSERT(sq_file(SQ_55) == FILE_5);
    
    ASSERT(sq_rank(SQ_11) == RANK_1);    
    ASSERT(sq_rank(SQ_12) == RANK_2);    
    ASSERT(sq_rank(SQ_13) == RANK_3);    
    ASSERT(sq_rank(SQ_14) == RANK_4);
    ASSERT(sq_rank(SQ_15) == RANK_5);

    ASSERT(sq_rank(SQ_21) == RANK_1);    
    ASSERT(sq_rank(SQ_22) == RANK_2);    
    ASSERT(sq_rank(SQ_23) == RANK_3);    
    ASSERT(sq_rank(SQ_24) == RANK_4);    
    ASSERT(sq_rank(SQ_25) == RANK_5);    

    ASSERT(sq_rank(SQ_31) == RANK_1);    
    ASSERT(sq_rank(SQ_32) == RANK_2);    
    ASSERT(sq_rank(SQ_33) == RANK_3);    
    ASSERT(sq_rank(SQ_34) == RANK_4);
    ASSERT(sq_rank(SQ_35) == RANK_5);

    ASSERT(sq_rank(SQ_41) == RANK_1);    
    ASSERT(sq_rank(SQ_42) == RANK_2);    
    ASSERT(sq_rank(SQ_43) == RANK_3);    
    ASSERT(sq_rank(SQ_44) == RANK_4);
    ASSERT(sq_rank(SQ_45) == RANK_5);

    ASSERT(sq_rank(SQ_51) == RANK_1);    
    ASSERT(sq_rank(SQ_52) == RANK_2);    
    ASSERT(sq_rank(SQ_53) == RANK_3);    
    ASSERT(sq_rank(SQ_54) == RANK_4);
    ASSERT(sq_rank(SQ_55) == RANK_5);

    auto h = HAND_NONE;

    ASSERT(!has_piece(h,PAWN));
    ASSERT(!has_piece(h,SILVER));
    ASSERT(!has_piece(h,GOLD));
    ASSERT(!has_piece(h,BISHOP));
    ASSERT(!has_piece(h,ROOK));

    auto h_pawn = inc_hand(h,PAWN);
    ASSERT(has_piece(h_pawn,PAWN));
    ASSERT(!has_piece(h_pawn,SILVER));
    ASSERT(!has_piece(h_pawn,GOLD));
    ASSERT(!has_piece(h_pawn,BISHOP));
    ASSERT(!has_piece(h_pawn,ROOK));

    auto h_pawn2 = inc_hand(h_pawn, PAWN);
    ASSERT(has_piece(h_pawn2,PAWN));
    ASSERT(!has_piece(h_pawn2,SILVER));
    ASSERT(!has_piece(h_pawn2,GOLD));
    ASSERT(!has_piece(h_pawn2,BISHOP));
    ASSERT(!has_piece(h_pawn2,ROOK));

    ASSERT(dec_hand(h_pawn2, PAWN) == h_pawn);
    ASSERT(dec_hand(h_pawn, PAWN) == h);

    auto h_silver = inc_hand(h, SILVER);
    ASSERT(!has_piece(h_silver,PAWN));
    ASSERT(has_piece(h_silver,SILVER));
    ASSERT(!has_piece(h_silver,GOLD));
    ASSERT(!has_piece(h_silver,BISHOP));
    ASSERT(!has_piece(h_silver,ROOK));

    auto h_silver2 = inc_hand(h_silver, SILVER);
    ASSERT(!has_piece(h_silver2,PAWN));
    ASSERT(has_piece(h_silver2,SILVER));
    ASSERT(!has_piece(h_silver2,GOLD));
    ASSERT(!has_piece(h_silver2,BISHOP));
    ASSERT(!has_piece(h_silver2,ROOK));

    ASSERT(dec_hand(h_silver2, SILVER) == h_silver);
    ASSERT(dec_hand(h_silver, SILVER) == h);

    auto h_gold = inc_hand(h, GOLD);
    ASSERT(!has_piece(h_gold,PAWN));
    ASSERT(!has_piece(h_gold,SILVER));
    ASSERT(has_piece(h_gold,GOLD));
    ASSERT(!has_piece(h_gold,BISHOP));
    ASSERT(!has_piece(h_gold,ROOK));

    auto h_gold2 = inc_hand(h_gold, GOLD);
    ASSERT(!has_piece(h_gold2,PAWN));
    ASSERT(!has_piece(h_gold2,SILVER));
    ASSERT(has_piece(h_gold2,GOLD));
    ASSERT(!has_piece(h_gold2,BISHOP));
    ASSERT(!has_piece(h_gold2,ROOK));

    ASSERT(dec_hand(h_gold2, GOLD) == h_gold);
    ASSERT(dec_hand(h_gold, GOLD) == h);

    auto h_bishop = inc_hand(h, BISHOP);
    ASSERT(!has_piece(h_bishop,PAWN));
    ASSERT(!has_piece(h_bishop,SILVER));
    ASSERT(!has_piece(h_bishop,GOLD));
    ASSERT(has_piece(h_bishop,BISHOP));
    ASSERT(!has_piece(h_bishop,ROOK));

    auto h_bishop2 = inc_hand(h_bishop, BISHOP);
    ASSERT(!has_piece(h_bishop2,PAWN));
    ASSERT(!has_piece(h_bishop2,SILVER));
    ASSERT(!has_piece(h_bishop2,GOLD));
    ASSERT(has_piece(h_bishop2,BISHOP));
    ASSERT(!has_piece(h_bishop2,ROOK));

    ASSERT(dec_hand(h_bishop2, BISHOP) == h_bishop);
    ASSERT(dec_hand(h_bishop, BISHOP) == h);

    auto h_rook = inc_hand(h, ROOK);
    ASSERT(!has_piece(h_rook,PAWN));
    ASSERT(!has_piece(h_rook,SILVER));
    ASSERT(!has_piece(h_rook,GOLD));
    ASSERT(!has_piece(h_rook,BISHOP));
    ASSERT(has_piece(h_rook,ROOK));

    auto h_rook2 = inc_hand(h_rook, ROOK);
    ASSERT(!has_piece(h_rook2,PAWN));
    ASSERT(!has_piece(h_rook2,SILVER));
    ASSERT(!has_piece(h_rook2,GOLD));
    ASSERT(!has_piece(h_rook2,BISHOP));
    ASSERT(has_piece(h_rook2,ROOK));

    ASSERT(dec_hand(h_rook2, ROOK) == h_rook);
    ASSERT(dec_hand(h_rook, ROOK) == h);


    auto h_all = inc_hand(h, PAWN);
    h_all = inc_hand(h_all, SILVER);
    h_all = inc_hand(h_all, GOLD);
    h_all = inc_hand(h_all, ROOK);
    h_all = inc_hand(h_all, BISHOP);
    
    ASSERT(has_piece(h_all,PAWN));
    ASSERT(has_piece(h_all,SILVER));
    ASSERT(has_piece(h_all,GOLD));
    ASSERT(has_piece(h_all,BISHOP));
    ASSERT(has_piece(h_all,ROOK));
    
    h_all = dec_hand(h_all, PAWN);
    h_all = dec_hand(h_all, SILVER);
    h_all = dec_hand(h_all, GOLD);
    h_all = dec_hand(h_all, BISHOP);
    h_all = dec_hand(h_all, ROOK);

    ASSERT(h_all == h);

    for(auto *fp = SQUARE_INDEX; *fp != SQ_WALL; ++fp) {
        // 持ち駒を打つ手
        const auto from = *fp;

        const auto mp = move(from, PAWN);
        const auto ms = move(from, SILVER);
        const auto mg = move(from, GOLD);
        const auto mb = move(from, BISHOP);
        const auto mr = move(from, ROOK);

        ASSERT(move_to(mp) == from);
        ASSERT(move_to(ms) == from);
        ASSERT(move_to(mg) == from);
        ASSERT(move_to(mb) == from);
        ASSERT(move_to(mr) == from);

        ASSERT(move_is_drop(mp));
        ASSERT(move_is_drop(ms));
        ASSERT(move_is_drop(mg));
        ASSERT(move_is_drop(mb));
        ASSERT(move_is_drop(mr));

        ASSERT(!move_is_prom(mp));
        ASSERT(!move_is_prom(ms));
        ASSERT(!move_is_prom(mg));
        ASSERT(!move_is_prom(mb));
        ASSERT(!move_is_prom(mr));

        ASSERT(move_is_ok(mp));
        ASSERT(move_is_ok(ms));
        ASSERT(move_is_ok(mg));
        ASSERT(move_is_ok(mb));
        ASSERT(move_is_ok(mr));

        ASSERT(sq_is_ok(from));
        
        ASSERT(sq_to_index(from) == sq_to_index_debug(from));

        // 盤上の駒を動かす手
        for(auto *tp = SQUARE_INDEX; *tp != SQ_WALL; ++tp) {
            const auto to = *tp;
            const auto m = move(from, to);
            const auto m2 = move(from, to, true);
            const auto from2 = move_from(m);
            const auto to2 = move_to(m);
            ASSERT(from == from2);
            ASSERT(to == to2);
            ASSERT(!move_is_prom(m));
            ASSERT(!move_is_drop(m));
            ASSERT(move_is_prom(m2));
            ASSERT(!move_is_drop(m2));
            ASSERT(move_is_ok(m));
            ASSERT(move_is_ok(m2));
        }
    }
    ASSERT(prom(PAWN) == PPAWN);
    ASSERT(prom(SILVER) == PSILVER);
    ASSERT(prom(BISHOP) == PBISHOP);
    ASSERT(prom(ROOK) == PROOK);
    ASSERT(unprom(PAWN) == PAWN);
    ASSERT(unprom(SILVER) == SILVER);
    ASSERT(unprom(GOLD) == GOLD);
    ASSERT(unprom(KING) == KING);
    ASSERT(unprom(BISHOP) == BISHOP);
    ASSERT(unprom(ROOK) == ROOK);

    ASSERT(unprom(PPAWN) == PAWN);
    ASSERT(unprom(PSILVER) == SILVER);
    ASSERT(unprom(PBISHOP) == BISHOP);
    ASSERT(unprom(PROOK) == ROOK);

    ASSERT(color_piece(PAWN,BLACK) == BLACK_PAWN);
    ASSERT(color_piece(SILVER,BLACK) == BLACK_SILVER);
    ASSERT(color_piece(GOLD,BLACK) == BLACK_GOLD);
    ASSERT(color_piece(BISHOP,BLACK) == BLACK_BISHOP);
    ASSERT(color_piece(ROOK,BLACK) == BLACK_ROOK);
    ASSERT(color_piece(KING,BLACK) == BLACK_KING);
    
    ASSERT(color_piece(PPAWN,BLACK) == BLACK_PPAWN);
    ASSERT(color_piece(PSILVER,BLACK) == BLACK_PSILVER);
    ASSERT(color_piece(PBISHOP,BLACK) == BLACK_PBISHOP);
    ASSERT(color_piece(PROOK,BLACK) == BLACK_PROOK);
    
    ASSERT(color_piece(PAWN,WHITE) == WHITE_PAWN);
    ASSERT(color_piece(SILVER,WHITE) == WHITE_SILVER);
    ASSERT(color_piece(GOLD,WHITE) == WHITE_GOLD);
    ASSERT(color_piece(BISHOP,WHITE) == WHITE_BISHOP);
    ASSERT(color_piece(ROOK,WHITE) == WHITE_ROOK);
    ASSERT(color_piece(KING,WHITE) == WHITE_KING);
    
    ASSERT(color_piece(PPAWN,WHITE) == WHITE_PPAWN);
    ASSERT(color_piece(PSILVER,WHITE) == WHITE_PSILVER);
    ASSERT(color_piece(PBISHOP,WHITE) == WHITE_PBISHOP);
    ASSERT(color_piece(PROOK,WHITE) == WHITE_PROOK);

    ASSERT(to_piece(WHITE_PAWN) == PAWN);
    ASSERT(to_piece(WHITE_SILVER) == SILVER);
    ASSERT(to_piece(WHITE_GOLD) == GOLD);
    ASSERT(to_piece(WHITE_KING) == KING);
    ASSERT(to_piece(WHITE_BISHOP) == BISHOP);
    ASSERT(to_piece(WHITE_ROOK) == ROOK);
    ASSERT(to_piece(WHITE_PPAWN) == PPAWN);
    ASSERT(to_piece(WHITE_PSILVER) == PSILVER);
    ASSERT(to_piece(WHITE_PBISHOP) == PBISHOP);
    ASSERT(to_piece(WHITE_PROOK) == PROOK);

    ASSERT(to_piece(BLACK_PAWN) == PAWN);
    ASSERT(to_piece(BLACK_SILVER) == SILVER);
    ASSERT(to_piece(BLACK_GOLD) == GOLD);
    ASSERT(to_piece(BLACK_KING) == KING);
    ASSERT(to_piece(BLACK_BISHOP) == BISHOP);
    ASSERT(to_piece(BLACK_ROOK) == ROOK);
    ASSERT(to_piece(BLACK_PPAWN) == PPAWN);
    ASSERT(to_piece(BLACK_PSILVER) == PSILVER);
    ASSERT(to_piece(BLACK_PBISHOP) == PBISHOP);
    ASSERT(to_piece(BLACK_PROOK) == PROOK);

    ASSERT(piece_color(WHITE_PAWN) == WHITE);
    ASSERT(piece_color(WHITE_SILVER) == WHITE);
    ASSERT(piece_color(WHITE_GOLD) == WHITE);
    ASSERT(piece_color(WHITE_KING) == WHITE);
    ASSERT(piece_color(WHITE_BISHOP) == WHITE);
    ASSERT(piece_color(WHITE_ROOK) == WHITE);
    ASSERT(piece_color(WHITE_PPAWN) == WHITE);
    ASSERT(piece_color(WHITE_PSILVER) == WHITE);
    ASSERT(piece_color(WHITE_PBISHOP) == WHITE);
    ASSERT(piece_color(WHITE_PROOK) == WHITE);

    ASSERT(piece_color(BLACK_PAWN) == BLACK);
    ASSERT(piece_color(BLACK_SILVER) == BLACK);
    ASSERT(piece_color(BLACK_GOLD) == BLACK);
    ASSERT(piece_color(BLACK_KING) == BLACK);
    ASSERT(piece_color(BLACK_BISHOP) == BLACK);
    ASSERT(piece_color(BLACK_ROOK) == BLACK);
    ASSERT(piece_color(BLACK_PPAWN) == BLACK);
    ASSERT(piece_color(BLACK_PSILVER) == BLACK);
    ASSERT(piece_color(BLACK_PBISHOP) == BLACK);
    ASSERT(piece_color(BLACK_PROOK) == BLACK);

    ASSERT2(dir_to_inc(0) == INC_UP,{ Tee<<dir_to_inc(0)<<":"<<INC_UP<<std::endl; });
    ASSERT(dir_to_inc(1) == INC_LEFTUP);
    ASSERT(dir_to_inc(2) == INC_LEFT);
    ASSERT(dir_to_inc(3) == INC_LEFTDOWN);
    ASSERT(dir_to_inc(4) == INC_DOWN);
    ASSERT(dir_to_inc(5) == INC_RIGHTDOWN);
    ASSERT(dir_to_inc(6) == INC_RIGHT);
    ASSERT(dir_to_inc(7) == INC_RIGHTUP);

    ASSERT(inc_to_dir(INC_UP) == 0);
    ASSERT(inc_to_dir(INC_LEFTUP) == 1);
    ASSERT(inc_to_dir(INC_LEFT) == 2);
    ASSERT(inc_to_dir(INC_LEFTDOWN) == 3);
    ASSERT(inc_to_dir(INC_DOWN) == 4);
    ASSERT(inc_to_dir(INC_RIGHTDOWN) == 5);
    ASSERT(inc_to_dir(INC_RIGHT) == 6);
    ASSERT(inc_to_dir(INC_RIGHTUP) == 7);
#endif
}
}
bool move_is_ok(const Move m, const game::Position &pos) {
    const auto to = move_to(m);
    if (!sq_is_ok(to)) {
        return false;
    }
    if (move_is_drop(m)) {
        const auto piece = move_piece(m);
        if (!piece_is_ok(piece)) {
            return false;
        }
        if (piece == KING || piece == PPAWN || piece == PSILVER || piece == PBISHOP || piece == PROOK) {
            return false;
        }
        if (pos.square(to) != COLOR_EMPTY) {
            return false;
        }
        if (piece == PAWN) {
            const auto rank = sq_rank(to);
            if (pos.turn() == BLACK && rank == RANK_1) {
                return false;
            }
            if (pos.turn() == WHITE && rank == RANK_5) {
                return false;
            }
            if (pos.exists_pawn(pos.turn(), sq_file(to))) {
                return false;
            }
        }
    } else {
        const auto from = move_from(m);
        const auto prom = move_is_prom(m);
        const auto color_piece = pos.square(from);
        const auto color = piece_color(color_piece);
        const auto piece = to_piece(color_piece);
        if (!sq_is_ok(from)) {
            return false;
        }
        if (color != pos.turn()) {
            return false;
        }
        if (prom) {
            if (piece == GOLD || piece == KING || piece == PPAWN || piece == PSILVER || piece == PBISHOP || piece == PROOK) {
                return false;
            }
        } 
        const auto cap = pos.square(to);
        if (cap != COLOR_EMPTY) {
            const auto cap_color = piece_color(cap);
            if (cap_color == pos.turn()) {
                return false;
            }
        }
    }
    return true;
}

#endif