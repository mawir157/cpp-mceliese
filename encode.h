#pragma once

#include "matrix.h"

void prepend_identity(matrix& rows, const size_t n_bits);
void append_identity(matrix& rows, const size_t n_bits);
matrix transpose(const matrix& m, const size_t n_bits);

code_word check_symbol(const code_word r, const matrix& check_code);
std::vector<code_word> check_message(const std::vector<code_word>& message,
                                     const matrix& check_code);

syndrome_table build_syn_table(const matrix& l_code,
															 const uint64_t width,
                               const uint64_t max_errors);
void print_syn_table(const syndrome_table& st, const size_t n_bits,
                     const size_t n_words);

std::vector<code_word> words_with_at_most_n_bits(const uint64_t n,
																								 const uint64_t max_bits);
void combs(std::vector<std::vector<size_t>>& cs, const std::vector<size_t>& ss,
           std::vector<size_t>& combination,
           const size_t offset, const size_t k);

code_word add_upto_n_error(const code_word& wd, const uint64_t width,
                           const uint64_t n);