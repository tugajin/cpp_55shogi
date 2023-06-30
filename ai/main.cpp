#include "game.hpp"
#include "sfen.hpp"
#include "movelist.hpp"
#include "movelegal.hpp"
#include "util.hpp"
// #include "search.hpp"
// #include "selfplay.hpp"
// #include "ubfm.hpp"
#include "attack.hpp"
// #include "matesearch.hpp"
#include "hash.hpp"
// #include "nn.hpp"
// #include "countreward.hpp"
// #include "reviewbuffer.hpp"
// #include "model.hpp"

TeeStream Tee;

int g_piece_color_piece[COLOR_SIZE][PIECE_END];

Square g_delta_inc_all[DELTA_NB];
Square g_delta_inc_line[DELTA_NB];
ColorPiece g_delta_mask[DELTA_NB];

namespace hash {
Key g_hash_pos[COLOR_SIZE][PIECE_END][SQ_END];
Key g_hash_hand[COLOR_SIZE][PIECE_END][3];
}

// namespace ubfm {
// UBFMSearcherGlobal g_searcher_global;
// }
// namespace selfplay {
// SelfPlayWorker g_selfplay_worker[SelfPlayWorker::NUM];
// int g_thread_counter;
// SelfPlayInfo g_selfplay_info;
// }

// namespace review {
// ReviewBuffer g_review_buffer;
// }
// namespace model {
// GPUModel g_gpu_model[GPUModel::GPU_NUM];
// }
int main(int argc, char **argv){
    auto num = 999999999;
    if (argc > 1) {
        num = std::stoi(std::string(argv[1]));
    }
    check_mode();
    init_table();
    hash::init();
    //attack::test_attack();
    gen::test_gen3();
    //gen::test_gen3();
    //oracle::g_oracle.load();
    //model::g_gpu_model[0].load_model(0);
    //model::test_oracle_model();
    //selfplay::execute_selfplay(num);
    //review::test_review();
    //model::test_model();
    return 0;
}