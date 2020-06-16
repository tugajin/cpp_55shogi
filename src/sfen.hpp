#ifndef SFEN_HPP
#define SFEN_HPP

class Pos;

const std::string START_SFEN = "rbsgk/4p/5/P4/KGSBR b - 1";

Pos pos_from_sfen(const std::string& s);
std::string out_sfen(const Pos& pos);

#endif