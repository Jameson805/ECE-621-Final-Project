#ifndef CORRECTION_HPP
#define CORRECTION_HPP

#include "lattice.hpp"
#include "decoder.hpp"
#include <vector>

void apply_correction(Lattice& lat, const std::vector<CorrectionMatch>& matches, DecoderGraph::Type type);

#endif
