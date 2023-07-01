#ifndef __MOVELEGAL_HPP__
#define __MOVELEGAL_HPP__

#include "game.hpp"
#include "movelist.hpp"
#include "movegen.hpp"
#include "movedrop.hpp"
#include <unordered_map>
//#include "moveevasion.hpp"
//#include "movecheck.hpp"
//#include "movecapture.hpp"
//#include "matesearch.hpp"

namespace gen {
template<bool is_exists = false> bool legal_moves(const game::Position &pos, movelist::MoveList &ml) {
    auto result = false;
    result = pos_moves<is_exists>(pos, ml);
    result |= drop_moves<is_exists>(pos, ml);
/*    if (attack::in_checked(pos)) {
        result = evasion_moves<is_exists>(pos, ml);
#if DEBUG
    if (!is_exists) {
        movelist::MoveList all_ml;
        pos_moves(pos, all_ml);
        REP(i, ml.len()) {
            auto next = pos.next(ml[i]);
            ASSERT2(!next.is_win(),{
                Tee<<pos<<std::endl;
                Tee<<next<<std::endl;
                Tee<<ml<<std::endl;
            });
        }
        movelist::MoveList tmp;
        REP(i, all_ml.len()) {
            auto next = pos.next(all_ml[i]);
            if (!next.is_win()) {
                tmp.add(all_ml[i]);
            }
        }
        ASSERT2(tmp.len() == ml.len(),{
            Tee<<pos<<std::endl;
            Tee<<tmp<<std::endl;
            Tee<<ml<<std::endl;
        })
    } else {
        movelist::MoveList tmp;
        evasion_moves(pos, tmp);
        ASSERT2(result == (tmp.len() !=0),{
            Tee<<pos<<std::endl;
            Tee<<tmp<<std::endl;
            Tee<<result<<std::endl;
        });
    }
#endif
    } else {
        result = pos_moves<is_exists>(pos, ml);
        result |= drop_moves<is_exists>(pos, ml);
#if DEBUG
        movelist::MoveList check_ml;
        pos_check_moves(pos, check_ml);
        drop_check_moves(pos, check_ml);
    if (!is_exists) {
        REP(i, ml.len()) {
            auto next = pos.next(ml[i]);
            if(attack::in_checked(next)) {
                if (!check_ml.contain(ml[i])) {
                    Tee<<pos<<std::endl;
                    Tee<<check_ml<<std::endl;
                    Tee<<move_str(ml[i])<<std::endl;
                    ASSERT(false);
                }
            }
        }
    } else {
        movelist::MoveList tmp;
        pos_moves(pos, tmp);
        drop_moves(pos, tmp);
        ASSERT(result == (tmp.len() !=0));
    }
#endif
    }
    */
    return result;
}

bool has_legal(const game::Position &pos) {
    movelist::MoveList dummy;
    return legal_moves<true>(pos, dummy);
}

int num_legal(const game::Position &pos) {
    movelist::MoveList dummy;
    legal_moves(pos, dummy);
    return dummy.len();
}

void test_gen2(std::string sfen) {
   auto pos = sfen::sfen(sfen);
   Tee<<pos<<std::endl;
   movelist::MoveList ml;
   legal_moves(pos,ml);
   Tee<<ml<<std::endl;
}

void test_gen() {
    test_gen2("rb1gk/1s2p/5/P3B/KGS1R b - 3");
    test_gen2(sfen::START_SFEN);
    test_gen2("4k/5/5/5/1P2K b P - 1");
    test_gen2("p3k/5/5/5/4K w p - 1");

}
void test_gen3() {
#if 1
    std::unordered_map<Key, int> key_dict;
    uint64 mate_num = 0;
    uint64 draw_num = 0;
    REP(i, 999999999999) {
        Tee<<i<<":"<<key_dict.size()<<" mate:"<<mate_num<<" draw:"<<draw_num<<"\r";
        auto pos = sfen::hirate();
        while(true) {
            //Tee<<pos<<std::endl;
            ASSERT2(pos.is_ok(),{
                Tee<<pos<<std::endl;
            });

            if (key_dict.count(hash::hash_key(pos)) == 0){
                key_dict.insert({hash::hash_key(pos), 1});
            }
            if (pos.is_draw()) {
                draw_num++;
                break;
            }
            if (pos.is_lose()) {
                mate_num++;
                break;
            }
            if (pos.is_win()) {
                mate_num++;
                break;
            }
            auto ml = movelist::MoveList();
            ml.init();
            legal_moves(pos, ml);
            if (ml.len() == 0) {
                mate_num++;
                break;
            }
            auto mv = ml[my_rand(ml.len())];
            //Tee<<move_str(mv)<<std::endl;
            pos = pos.next(mv);
        }
    }
#endif
}

}
namespace game {
bool Position::is_lose() const {
    return !gen::has_legal(*this);
}
}
#endif