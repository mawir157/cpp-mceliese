#pragma once

#include "matrix.h"

typedef std::map<code_word, code_word> syndrome_table;

void prepend_identity(matrix& rows, const uint64_t n_bits);
void append_identity(matrix& rows, const uint64_t n_bits);
matrix transpose(const matrix& m, const uint64_t n_bits);

code_word check_symbol(const code_word r, const matrix& check_code);
std::vector<code_word> check_message(const std::vector<code_word>& message,
                                     const matrix& check_code);

syndrome_table build_syn_table(const matrix& l_code,
															 const uint64_t width,
                               const uint64_t max_errors);
void print_syn_table(const syndrome_table& st, const uint64_t n_bits);

std::vector<code_word> words_with_at_most_n_bits(const uint64_t n,
																								 const uint64_t max_bits);

code_word add_upto_n_error(const code_word& wd, const uint64_t width,
                           const uint64_t n);