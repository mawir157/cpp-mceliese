#pragma once

#include "matrix.h"

matrix canonise(const matrix& cw);

void recursively_build(matrix rows,
                       size_t depth,
                       const size_t cur_index,
                       const std::vector<code_word> odd_words,
                       std::vector<matrix>& matrices,
                       const bool verbose);

std::vector<matrix> all(const uint64_t n, const bool verbose);

std::vector<code_word> basis_span(const matrix& generator);

std::vector<size_t> v_space_sig(const matrix& generator);

std::map<size_t, size_t> sig_table(const matrix& generator);

matrix semi_direct_product(const matrix& m1, const matrix& m2,
                           const size_t n_bits);

std::vector<matrix> all_divide_and_conquer(const uint64_t n,
                                           const bool verbose=false);
