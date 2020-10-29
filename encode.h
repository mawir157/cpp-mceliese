#pragma once

#include "matrix.h"

typedef std::map<code_word, code_word> syndrome_table;

void prepend_identity(matrix& rows);
void append_identity(matrix& rows);

code_word check_symbol(const code_word r, const matrix& check_code);
std::vector<code_word> check_message(const std::vector<code_word>& message,
                                     const matrix& check_code);

syndrome_table build_syn_table(const matrix& l_code,
                               const unsigned int max_errors);
void print_syn_table(syndrome_table& st, const unsigned int n_bits);

std::vector<code_word> words_with_at_most_n_bits(const unsigned int n,
																								 const unsigned int max_bits);

code_word add_upto_n_error(const code_word& wd, const unsigned int width,
                           const unsigned int n);