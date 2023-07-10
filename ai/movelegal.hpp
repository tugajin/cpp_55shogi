#ifndef __MOVELEGAL_HPP__
#define __MOVELEGAL_HPP__

#include "game.hpp"
#include "movelist.hpp"
#include "movegen.hpp"
#include "movedrop.hpp"
#include <unordered_map>
#include "moveevasion.hpp"
#include "movecheck.hpp"
//#include "movecapture.hpp"
//#include "matesearch.hpp"

namespace gen {

template<bool is_exists = false> bool legal_moves(game::Position &pos, movelist::MoveList &ml) {
    ASSERT2(pos.is_ok(),{
        Tee<<"pos error\n";
        Tee<<pos<<std::endl;
    });
    ASSERT2(!pos.is_win(),{
        Tee<<"pos is win\n";
        Tee<<pos<<std::endl;
    });
    auto result = false;
    if (attack::in_checked(pos)) {
        result = evasion_moves<is_exists>(pos, ml);
#if 1
    if (!is_exists) {
        movelist::MoveList all_ml;
        pos_moves<false,false>(pos, all_ml);
        drop_moves<false>(pos, all_ml);
        movelist::MoveList tmp;
        REP(i, all_ml.len()) {
            auto next = pos.next(all_ml[i]);
            if (!next.is_win()) {
                tmp.add(all_ml[i]);
            }
        }
        ASSERT2(tmp.len() == ml.len(),{
            Tee<<pos<<std::endl;
            Tee<<"correct:\n";
            Tee<<tmp<<std::endl;
            Tee<<"incorrect:\n";
            Tee<<ml<<std::endl;
        })
    } else {
        movelist::MoveList tmp;
        evasion_moves<false>(pos, tmp);
        ASSERT2(result == (tmp.len() !=0),{
            Tee<<pos<<std::endl;
            Tee<<tmp<<std::endl;
            Tee<<result<<std::endl;
        });
    }
#endif
    } else {
        result = pos_moves<is_exists,true>(pos, ml);
        result |= drop_moves<is_exists>(pos, ml);
#if 1
        if (!is_exists) {
            movelist::MoveList all_ml;
            pos_moves<false,false>(pos, all_ml);
            drop_moves<false>(pos, all_ml);
            movelist::MoveList tmp;
            REP(i, all_ml.len()) {
                auto next = pos.next(all_ml[i]);
                if (!next.is_win()) {
                    tmp.add(all_ml[i]);
                }
            }
            ASSERT2(tmp.len() == ml.len(),{
                Tee<<pos<<std::endl;
                Tee<<"correct:\n";
                Tee<<tmp<<std::endl;
                Tee<<"incorrect:\n";
                Tee<<ml<<std::endl;
            });
        } else {
            movelist::MoveList tmp;
            pos_moves<false,true>(pos, tmp);
            drop_moves<false>(pos, tmp);
            ASSERT2(result == (tmp.len() !=0),{
                Tee<<pos<<std::endl;
                Tee<<tmp<<std::endl;
                Tee<<result<<std::endl;
            }); 
        }
#endif
    }
    return result;
}

void check_moves(game::Position &pos, movelist::MoveList &ml) {
    ASSERT2(!attack::in_checked(pos),{
        Tee<<pos<<std::endl;
    });
    drop_check_moves(pos,ml);
    pos_check_moves(pos,ml);
#if 1
    auto pos2 = pos;
    movelist::MoveList all_ml;
    legal_moves(pos2,all_ml);
    REP(i, all_ml.len()) {
        auto next = pos2.next(all_ml[i]);
        if (attack::in_checked(next)) {
            if (!ml.contain(all_ml[i])) {
                Tee<<"check moves error\n";
                Tee<<pos2<<std::endl;
                Tee<<move_str(all_ml[i])<<std::endl;
                Tee<<"-------------------\n";
                Tee<<ml<<std::endl;
                ASSERT(false);
            }
        }
    }
    REP(i, ml.len()) {
        if (!all_ml.contain(ml[i])) {
            Tee<<pos2<<std::endl;
            Tee<<move_str(ml[i])<<std::endl;
            ASSERT(false);
        }
    }
#endif
}

bool has_legal(game::Position &pos) {
    movelist::MoveList dummy;
    return legal_moves<true>(pos, dummy);
}

int num_legal(game::Position &pos) {
    movelist::MoveList dummy;
    legal_moves(pos, dummy);
    return dummy.len();
}

bool test_gen2(std::string sfen, const int num, const bool check = false) {
    auto pos = sfen::sfen(sfen);
    Tee<<pos<<std::endl;
    movelist::MoveList ml;
    if (check) {
        check_moves(pos,ml);
    } else {
        legal_moves(pos,ml);
    }
    Tee<<ml<<std::endl;
    if (ml.len() != num) {
        Tee<<"not eq:"<<ml.len()<<":"<<num<<std::endl;
        return false;
    }
    return true;
}

void test_gen() {
    ASSERT(test_gen2("pb2k/b3r/5/KS1Sr/4g w pg - 52",5,true));
    ASSERT(test_gen2("4g/2g2/p1SBk/Sr1R1/4K b Pb - 9",2,true));
    ASSERT(test_gen2("r1sgk/b3R/1B3/P4/KGS2 w P - 4",2));
    ASSERT(test_gen2("2k2/5/2K2/5/2R2 b - 1", 4,true));
    ASSERT(test_gen2("1r1k1/1gs2/2r1p/1K1Sb/1GPB1 w - 42", 7,true));
    ASSERT(test_gen2("5/5/2k2/5/2K2 b PSGBR - 1", 27,true));
    ASSERT(test_gen2("2s1k/2b1g/rB3/P2S1/KG3 b Pr - 1",0,true));
    ASSERT(test_gen2("2B2/1P3/2k2/5/2K2 b - 1", 1,true));
    ASSERT(test_gen2("2S2/1P3/2k2/5/2K2 b - 1", 1,true));
    ASSERT(test_gen2("2B2/1p3/2k2/5/2K2 b - 1", 2,true));
    ASSERT(test_gen2("2S2/1p3/2k2/5/2K2 b - 1", 2,true));
    ASSERT(test_gen2("2k2/5/2G2/2R2/2K2 b - 1", 5,true));
    ASSERT(test_gen2("2k2/5/2B2/2R2/2K2 b - 1", 8,true));
    ASSERT(test_gen2("2k2/5/5/2G2/1KR2 b - 1", 4,true));
    ASSERT(test_gen2("2k2/5/5/2B2/1KR2 b - 1", 5,true));
    ASSERT(test_gen2("2k2/1P3/5/5/2K2 b - 1", 1,true));
    ASSERT(test_gen2("2sk1/1P3/rBK2/S3+r/3g1 w pbg - 66",6));
    ASSERT(test_gen2("2k1p/2Grr/p3B/1S1+s1/K2gb w - 116",1));
    ASSERT(test_gen2("2SB1/5/2b1r/PRs1k/Kgg2 b p - 91",0));
    ASSERT(test_gen2("1b2+R/B1Ps1/PrG1k/3g1/K1S2 w - 50",1));
    ASSERT(test_gen2("4k/1rS2/3gp/R1p1b/1GKB1 b s - 27",3));
    ASSERT(test_gen2("s3k/4p/K1rbS/5/1G1B1 b PGr - 25",5));
    ASSERT(test_gen2("r2gk/4p/s1S2/PK2b/1G1BR b - 7",5));
    ASSERT(test_gen2("gk1g1/1rSRp/P2b1/4b/Ks3 w - 50",3));
    ASSERT(test_gen2("r3k/b2sp/Kg1G1/P4/2SBR b - 9",1));
    ASSERT(test_gen2("+SK2k/1G3/3+b1/RPp1+r/1Sg2 b b - 61",3));
    ASSERT(test_gen2("r1sg1/3k1/K3p/P2bR/1GS+b1 b - 11",1));
    ASSERT(test_gen2("1bsg1/3k1/4R/rSP2/K2B1 b Gp - 13",2));
    ASSERT(test_gen2("rb1gk/1s2p/5/P3B/KGS1R b - 3",12));
    ASSERT(test_gen2(sfen::START_SFEN,14));
    ASSERT(test_gen2("4k/5/5/5/1P2K b P - 1",19));
    ASSERT(test_gen2("p3k/5/5/5/4K w p - 1",19));
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
            if (pos.is_win()) {
                mate_num++;
                break;
            }
            if (pos.is_lose()) {
                mate_num++;
                break;
            }
            auto ml = movelist::MoveList();
            legal_moves(pos, ml);
            if (ml.len() == 0) {
                mate_num++;
                break;
            }
            if (!attack::in_checked(pos)) {
                auto ml2 = movelist::MoveList();
                check_moves(pos,ml2);
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
bool Position::is_lose() {
    return !gen::has_legal(*this);
}
}
#endif